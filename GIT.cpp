#include "GIT.h"

#include <iostream>


void GIT::printErrorAndShutdown(std::string text)
{
	std::cerr << text << std::endl;
	git_libgit2_shutdown();
	exit(EXIT_FAILURE);
}

void GIT::getLastError(std::string info)
{

	const git_error* lastError = git_error_last();
	if (lastError != nullptr)
		printErrorAndShutdown(info + lastError->message);
	else
		printErrorAndShutdown(info + "Unknown error messasge");
}
void GIT::clearGitIgnore()
{

}
void GIT::appendGitIgnore(const std::vector<std::string>& ignorePatterns)
{
	std::string gitIgnorePath = repoPath + "/.gitignore";
	std::set<std::string> existingPatterns;

	std::ifstream gitIgnoreFileIn(gitIgnorePath);
	if (gitIgnoreFileIn.is_open())
	{
		std::string line;
		while (std::getline(gitIgnoreFileIn, line))
			existingPatterns.insert(line);
		gitIgnoreFileIn.close();
	}

	std::ofstream gitIgnoreFileOut(gitIgnorePath, std::ios::app);
	if (!gitIgnoreFileOut.is_open())
	{
		printErrorAndShutdown("gitIgnoreFileOut open failed!");
	}

	for (const auto& pattern : ignorePatterns)
	{
		if (existingPatterns.find(pattern) == existingPatterns.end())
		{
			gitIgnoreFileOut << pattern << "\n";
			existingPatterns.insert(pattern);
		}
	}
	gitIgnoreFileOut.close();
}


GIT::FileStatus GIT::collectRepoStatus()
{
	GIT::FileStatus fileStatus;

	git_status_options opts;
	if (git_status_options_init(&opts, GIT_STATUS_OPTIONS_VERSION) < 0)
		getLastError("git_status_options_init failed: ");
	opts.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR; // Include untracked files
	opts.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED; // Explicitly include untracked files

	git_status_list* statusList;
	if (git_status_list_new(&statusList, repo, &opts) < 0)
		getLastError("git_status_list_new failed: ");

	size_t statusCount = git_status_list_entrycount(statusList);
	std::cout << "git_status_list_entrycount: " + std::to_string(statusCount) << std::endl;
	for (size_t i = 0; i < statusCount; i++)
	{
		const git_status_entry* entry = git_status_byindex(statusList, i);
		if (entry->status & GIT_STATUS_WT_NEW)
		{
			if (entry->index_to_workdir)
				fileStatus.untracked.push_back(entry->index_to_workdir->new_file.path);
		}
		else if (entry->status & GIT_STATUS_WT_MODIFIED)
		{
			if (entry->head_to_index)
				fileStatus.modified.push_back(entry->head_to_index->new_file.path);
		}
		else if (entry->status & GIT_STATUS_WT_DELETED)
		{
			if (entry->head_to_index)
				fileStatus.deleted.push_back(entry->head_to_index->old_file.path);
		}
	}

	git_status_list_free(statusList);

	return fileStatus;
}


std::string GIT::printRepoStatus(const GIT::FileStatus& fileStatus)
{
	std::ostringstream oss;
	for (const auto& file : fileStatus.untracked)
		oss << "Untracked files:\t" << file << "\n";
	for (const auto& file : fileStatus.modified)
		oss << "Modified files:\t" << file << "\n";
	for (const auto& file : fileStatus.deleted)
		oss << "Deleted files:\t" << file << "\n";

	return oss.str();
}

void GIT::stagingFiles(std::vector<std::string> filesPath)
{
	git_index* index = nullptr;
	if (git_repository_index(&index, repo) < 0)
		getLastError("get_repository_index failed: ");

	for (const auto& filePath : filesPath)
	{
		if (git_index_add_bypath(index, filePath.c_str()) < 0)
			getLastError("git_index_add_bypath failed at " + filePath + ": ");
		std::cout << "Added file to index: " + filePath << std::endl;
	}


	switch (git_index_write(index))
	{
		git_index_free(index);
	case 0:
		break;
	default:
		getLastError("git_index_write failed: ");
		break;
	}
}
void GIT::stagingAllUntrackedFiles()
{
	auto fileStatus = collectRepoStatus();
	stagingFiles(fileStatus.untracked);
}
void GIT::stagingAllModifiedFiles()
{
	auto fileStatus = collectRepoStatus();
	stagingFiles(fileStatus.modified);
}
void GIT::stagingAllDeletedFiles()
{
	auto fileStatus = collectRepoStatus();
	stagingFiles(fileStatus.deleted);
}
void GIT::stagingAll()
{
	auto fileStatus = collectRepoStatus();
	stagingFiles(fileStatus.untracked);
	stagingFiles(fileStatus.modified);
	stagingFiles(fileStatus.deleted);
}

void GIT::commitCurrentStage(std::string commit_message)
{
	git_index* index = nullptr;
	if (git_repository_index(&index, repo) < 0)
		getLastError("git_repository_index failed: ");

	git_oid tree_oid;
	if (git_index_write_tree(&tree_oid, index) < 0)
		getLastError("git_index_write_tree");

	git_tree* tree = nullptr;
	if (git_tree_lookup(&tree, repo, &tree_oid) < 0)
		getLastError("git_tree_lookup failed: ");

	git_signature* sig = nullptr;
	if (git_signature_now(&sig, userName.c_str(), userEmail.c_str()) < 0)
		getLastError("git_signature_now failed: ");

	git_oid parent_commit_oid;
	git_commit* parent_commit = nullptr;
	if (git_reference_name_to_id(&parent_commit_oid, repo, "HEAD") == GIT_OK)
	{
		if (git_commit_lookup(&parent_commit, repo, &parent_commit_oid) < 0)
			getLastError("git_commit_lookup failed: ");
	}
	git_oid commit_oid;
	if (git_commit_create_v(
		&commit_oid,
		repo,
		"HEAD",
		sig,
		sig,
		nullptr,
		commit_message.c_str(),
		tree,
		parent_commit ? 1 : 0,
		parent_commit ? (const git_commit**)&parent_commit : nullptr
	) < 0)
		getLastError("git_commit_create_v failed: ");

	git_signature_free(sig);
	git_tree_free(tree);
	git_index_free(index);
	if (parent_commit) git_commit_free(parent_commit);

	
}

GIT::GIT(std::string repoPath, std::string userName, std::string userEmail)
	: repoPath(repoPath), userName(userName), userEmail(userEmail)
{

	git_libgit2_init();
	auto t0 = std::chrono::system_clock::now();
	while (git_repository_open(&repo, repoPath.c_str()) != GIT_OK)
	{
		if (std::chrono::system_clock::now() - t0 > std::chrono::seconds(GIT_TIMEOUT))
		{
			printErrorAndShutdown("git_repository_open timeout!");
		}
		if (git_repository_init(&repo, repoPath.c_str(), false) < 0)
			getLastError("git_repository_init failed: ");
		std::cout << "git_repository_init success" << std::endl;

		clearGitIgnore();
		appendGitIgnore(ignorePreset);

		stagingAll();



	}
	std::cout << "git_repository_open success" << std::endl;
}


bool GIT::isRepoExist(std::string repoPath)
{
	bool  ret = false;
	git_repository* repoTemp = nullptr;
	git_libgit2_init();
	int git_ret = git_repository_open(&repoTemp, repoPath.c_str());
	if (git_ret == GIT_OK)
		ret = true;
	else
		ret = false;

	git_libgit2_shutdown();
	return ret;
}

GIT::~GIT()
{
	git_repository_free(repo);
	git_libgit2_shutdown();
}