#include "GIT.h"

#include <filesystem>

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
	std::string gitIgnorePath = repoPath + "/.gitignore";
	std::ifstream gitIgnoreFileTrunc(gitIgnorePath, std::ios::trunc);
	if (gitIgnoreFileTrunc.is_open())
	{
		std::cout << "git ignore file successfully clear" << std::endl;
		gitIgnoreFileTrunc.close();
	}
	else
		std::cout << "git ignore file clear failed" << std::endl;

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
	//std::cout << "git_status_list_entrycount: " + std::to_string(statusCount) << std::endl;
	for (size_t i = 0; i < statusCount; i++)
	{
		const git_status_entry* entry = git_status_byindex(statusList, i);		
		if (entry->status & GIT_STATUS_WT_NEW)
		{
			fileStatus.notStaged.newFiles.push_back(entry->index_to_workdir->new_file.path);
			continue;
		}
		if (entry->status & GIT_STATUS_WT_MODIFIED)
		{
			fileStatus.notStaged.modifiedFiles.push_back(entry->index_to_workdir->new_file.path);
			continue;
		}
		if (entry->status & GIT_STATUS_WT_DELETED)
		{
			fileStatus.notStaged.deletedFiles.push_back(entry->index_to_workdir->new_file.path);
			continue;
		}
		if (entry->status & GIT_STATUS_WT_TYPECHANGE)
		{
			fileStatus.notStaged.typechangedFiles.push_back(entry->index_to_workdir->new_file.path);
			continue;
		}
		if (entry->status & GIT_STATUS_WT_RENAMED)
		{
			fileStatus.notStaged.renamedFiles.push_back(entry->index_to_workdir->new_file.path);
			continue;
		}
		if (entry->status & GIT_STATUS_WT_UNREADABLE)
		{
			fileStatus.notStaged.unreadableFiles.push_back(entry->index_to_workdir->new_file.path);
			continue;
		}
		if (entry->status & GIT_STATUS_INDEX_NEW)
		{
			fileStatus.staged.newFiles.push_back(entry->head_to_index->new_file.path);
			continue;
		}
		if (entry->status & GIT_STATUS_INDEX_MODIFIED)
		{
			fileStatus.staged.modifiedFiles.push_back(entry->head_to_index->new_file.path);
			continue;
		}
		if (entry->status & GIT_STATUS_INDEX_DELETED)
		{
			fileStatus.staged.deletedFiles.push_back(entry->head_to_index->new_file.path);
			continue;
		}
		if (entry->status & GIT_STATUS_INDEX_RENAMED)
		{
			fileStatus.staged.renamedFiles.push_back(entry->head_to_index->new_file.path);
			continue;
		}
		if (entry->status & GIT_STATUS_INDEX_TYPECHANGE)
		{
			fileStatus.staged.typechangedFiles.push_back(entry->head_to_index->new_file.path);
			continue;
		}

	}

	git_status_list_free(statusList);

	return fileStatus;
}


std::string GIT::printRepoStatus(const GIT::FileStatus& fileStatus)
{
	std::ostringstream oss;
	oss << "\nNot Staged:\n";
	for (const auto& file : fileStatus.notStaged.newFiles)
		oss << "\tUntracked files:\t" << file << "\n";
	for (const auto& file : fileStatus.notStaged.modifiedFiles)
		oss << "\tModified files:\t" << file << "\n";
	for (const auto& file : fileStatus.notStaged.deletedFiles)
		oss << "\tDeleted files:\t" << file << "\n";
	for (const auto& file : fileStatus.notStaged.renamedFiles)
		oss << "\tRenamed files:\t" << file << "\n";
	for (const auto& file : fileStatus.notStaged.typechangedFiles)
		oss << "\tTypechanged files:\t" << file << "\n";
	for (const auto& file : fileStatus.notStaged.unreadableFiles)
		oss << "\tUnreadable files:\t" << file << "\n";
	oss << "\nStaged:\n";
	for (const auto& file : fileStatus.staged.newFiles)
		oss << "\New files:\t" << file << "\n";
	for (const auto& file : fileStatus.staged.modifiedFiles)
		oss << "\tModified files:\t" << file << "\n";
	for (const auto& file : fileStatus.staged.deletedFiles)
		oss << "\tDeleted files:\t" << file << "\n";
	for (const auto& file : fileStatus.staged.renamedFiles)
		oss << "\tRenamed files:\t" << file << "\n";
	for (const auto& file : fileStatus.staged.typechangedFiles)
		oss << "\tTypechanged files:\t" << file << "\n";

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
	stagingFiles(fileStatus.notStaged.newFiles);
}
void GIT::stagingAllModifiedFiles()
{
	auto fileStatus = collectRepoStatus();
	stagingFiles(fileStatus.notStaged.modifiedFiles);
}
void GIT::stagingAllDeletedFiles()
{
	auto fileStatus = collectRepoStatus();
	stagingFiles(fileStatus.notStaged.deletedFiles);
}
void GIT::stagingAllRenamedFiles()
{
	auto fileStatus = collectRepoStatus();
	stagingFiles(fileStatus.notStaged.renamedFiles);
}
void GIT::stagingAllTypechangedFiles()
{
	auto fileStatus = collectRepoStatus();
	stagingFiles(fileStatus.notStaged.typechangedFiles);
}
// except unreadable files

void GIT::stagingAll()
{
	auto fileStatus = collectRepoStatus();
	std::vector<std::string> notStagedFiles;
	notStagedFiles.insert(notStagedFiles.end(), fileStatus.notStaged.newFiles.begin(), fileStatus.notStaged.newFiles.end());
	notStagedFiles.insert(notStagedFiles.end(), fileStatus.notStaged.modifiedFiles.begin(), fileStatus.notStaged.modifiedFiles.end());
	notStagedFiles.insert(notStagedFiles.end(), fileStatus.notStaged.deletedFiles.begin(), fileStatus.notStaged.deletedFiles.end());
	notStagedFiles.insert(notStagedFiles.end(), fileStatus.notStaged.renamedFiles.begin(), fileStatus.notStaged.renamedFiles.end());
	notStagedFiles.insert(notStagedFiles.end(), fileStatus.notStaged.typechangedFiles.begin(), fileStatus.notStaged.typechangedFiles.end());
	stagingFiles(notStagedFiles);
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

	if (!std::filesystem::is_directory(repoPath))
	{
		std::cout << "Repo Path directory is not exist" << std::endl;
		if (std::filesystem::create_directory(repoPath))
			std::cout << "Create dir: " << repoPath << std::endl;		
	}

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

std::string GIT::getCurrentBranch()
{
	git_reference* head_ref = nullptr;
	int ret = git_repository_head(&head_ref, repo);	
	switch (ret)
	{
	case GIT_EUNBORNBRANCH:
		std::cout << "No commits yet (unborn branch)" << std::endl;
		return "";
		break;
	case GIT_ENOTFOUND:
		std::cout << "No branch found (detached HEAD or empty repository)" << std::endl;
		return "";
		break;
	case GIT_OK:
		
		break;
	default:
		getLastError("git_repository_head failed: ");
		break;
	}

	// only GIT_OK case escape from switch
	const char* branch_name = nullptr;
	if (git_branch_name(&branch_name, head_ref) < 0)
		getLastError("git_branch_name failed: ");

	git_reference_free(head_ref);
	return std::string(branch_name);

}

std::string GIT::getCurrentStatus()
{
	auto fileStatus = collectRepoStatus();
	return printRepoStatus(fileStatus);
}

void GIT::createBranch(const std::string& branch_name)
{
	git_reference* new_branch = nullptr;
	git_object* head_commit = nullptr;
	if (git_revparse_single(&head_commit, repo, "HEAD") < 0)
		getLastError("git_revparse_single failed: ");
	if (git_branch_create(&new_branch, repo, branch_name.c_str(), (git_commit*)head_commit, 0) < 0)
		getLastError("git_branch_create failed: ");

	git_object_free(head_commit);
	git_reference_free(new_branch);

}
void GIT::switchBranch(const std::string& branch_name)
{
	git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
	opts.checkout_strategy = GIT_CHECKOUT_SAFE;

	git_reference* branch_ref = nullptr;
	if (git_branch_lookup(&branch_ref, repo, branch_name.c_str(), GIT_BRANCH_LOCAL) < 0)
		getLastError("git_branch_lookup failed: ");

	if (git_repository_set_head(repo, git_reference_name(branch_ref)) < 0)
		getLastError("git_repository_set_head failed: ");

	if (git_checkout_head(repo, &opts) < 0)
		getLastError("git_checkout_head failed: ");

	git_reference_free(branch_ref);
}
void GIT::mergeBranch(const std::string& source_branch)
{
	git_reference* target_ref = nullptr;
	git_reference* source_ref = nullptr;
	git_annotated_commit* source_annotated = nullptr;

	if (git_repository_head(&target_ref, repo) < 0)
		getLastError("git_repository_head failed: ");

	if (git_branch_lookup(&source_ref, repo, source_branch.c_str(), GIT_BRANCH_LOCAL) < 0)
		getLastError("git_branch_lookup failed: ");

	if(git_annotated_commit_from_ref(&source_annotated, repo, source_ref) < 0)
		getLastError("git_annotated_commit_from_ref failed: ");

	if(git_merge(repo, (const git_annotated_commit**)&source_annotated, 1, nullptr, nullptr) < 0)
		getLastError("git_merge failed: ");
	
	git_reference_free(target_ref);
	git_reference_free(source_ref);
	git_annotated_commit_free(source_annotated);
}
void GIT::deleteBranch(const std::string& branch_name)
{
	git_reference* branch_ref = nullptr;

	if(git_branch_lookup(&branch_ref, repo, branch_name.c_str(), GIT_BRANCH_LOCAL) < 0)
		getLastError("git_branch_lookup failed: ");

	if(git_branch_delete(branch_ref) < 0)
		getLastError("git_branch_delete failed: ");
	
	git_reference_free(branch_ref);
}

std::vector<std::string> GIT::getBranchList(git_branch_t branch_type_enum)
{
	std::vector<std::string> branch_list;

	git_branch_iterator* iter = nullptr;
	git_reference* branch_ref = nullptr;
	git_branch_t branch_type;
	if (git_branch_iterator_new(&iter, repo, branch_type_enum) < 0)
		getLastError("git_branch_iterator_new failed: ");
	while (git_branch_next(&branch_ref, &branch_type, iter) != GIT_ITEROVER)
	{
		const char* branch_name = nullptr;
		if (git_branch_name(&branch_name, branch_ref) < 0)
			getLastError("git_branch_name failed: ");
		branch_list.emplace_back(branch_name);

		git_reference_free(branch_ref);
	}
	git_branch_iterator_free(iter);

	return branch_list;
}
std::vector<std::string> GIT::getLocalBranchList()
{
	return getBranchList(GIT_BRANCH_LOCAL);
}
std::vector<std::string> GIT::getRemoteBranchList()
{
	return getBranchList(GIT_BRANCH_REMOTE);
}
std::vector<std::string> GIT::getAllBranchList()
{
	return getBranchList(GIT_BRANCH_ALL);
}




git_diff_file_cb GIT::git_diff_file_callback = [](const git_diff_delta* delta, float progress, void* payload)->int
{
	auto diffResults = (std::vector<GIT::DiffResult>*)payload;
	GIT::DiffResult diffResult;
	diffResult.filePath = std::string(delta->new_file.path);
	diffResults->push_back(diffResult);

	//std::cout << "git_diff_file_callback " + std::string(delta->new_file.path) << std::endl;
	return 0;
};
git_diff_hunk_cb GIT::git_diff_hunk_callback = [](const git_diff_delta* delta, const git_diff_hunk* hunk, void* payload)->int
{
	auto diffResults = (std::vector<GIT::DiffResult>*)payload;
	if (diffResults->empty())
	{
		//std::cout << "git_diff_hunk_callback diffResults->empty()" << std::endl;
		return -1;
	}

	GIT::DiffResult* diffResult = &diffResults->back();
	if (diffResult->filePath != std::string(delta->new_file.path))
	{
		//std::cout << "git_diff_hunk_callback diffResult->filePath != std::string(delta->new_file.path)" << std::endl;
		return -1;
	}

	DiffHunk diffHunk;
	diffHunk.hunk = *hunk;
	diffResult->diffHunks.push_back(diffHunk);
	diffResult->current_new_line_index = hunk->new_start;
	diffResult->current_old_line_index = hunk->old_start;

	//std::cout << "git_diff_hunk_callback @@ -" << hunk->old_start << "," << hunk->old_lines << " +" << hunk->new_start << "," << hunk->new_lines << " @@" << std::endl;
	return 0;
};
git_diff_line_cb GIT::git_diff_line_callback = [](const git_diff_delta* delta, const git_diff_hunk* hunk, const git_diff_line* line, void* payload)->int
{
	auto diffResults = (std::vector<GIT::DiffResult>*)payload;
	if (diffResults->empty())
	{
		//std::cout << "git_diff_line_callback diffResults->empty()" << std::endl;
		return -1;
	}

	GIT::DiffResult* diffResult = &diffResults->back();
	if (diffResult->filePath != std::string(delta->new_file.path))
	{
		//std::cout << "git_diff_line_callback diffResult->filePath != std::string(delta->new_file.path)" << std::endl;
		return -1;
	}

	if (diffResult->diffHunks.empty())
	{
		//std::cout << "git_diff_line_callback diffResult->diffHunks.empty()" << std::endl;
		return -1;
	}

	DiffHunk* diffHunk = &diffResult->diffHunks.back();
	if ((diffHunk->hunk.new_start > diffResult->current_new_line_index) || ((diffHunk->hunk.new_start + diffHunk->hunk.new_lines) < diffResult->current_new_line_index)) // exactly start + lines - 1 < index
	{
		//std::cout << "git_diff_line_callback " << diffHunk->hunk.new_start << ", " << diffResult->current_new_line_index << ", " << diffHunk->hunk.new_start + diffHunk->hunk.new_lines - 1 << " (diffHunk->hunk.new_start > diffResult->current_new_line_index) || ((diffHunk->hunk.new_start + diffHunk->hunk.new_lines - 1) < diffResult->current_new_line_index)" << std::endl;
		return -1;
	}
	if ((diffHunk->hunk.old_start > diffResult->current_old_line_index) || ((diffHunk->hunk.old_start + diffHunk->hunk.old_lines) < diffResult->current_old_line_index)) // exactly start + lines - 1 < index
	{
		//std::cout << "git_diff_line_callback " << diffHunk->hunk.old_start << ", " << diffResult->current_old_line_index << ", " << diffHunk->hunk.old_start + diffHunk->hunk.old_lines - 1 << " (diffHunk->hunk.old_start > diffResult->current_old_line_index) || ((diffHunk->hunk.old_start + diffHunk->hunk.old_lines - 1) < diffResult->current_old_line_index)" << std::endl;
		return -1;
	}



	switch (line->origin)
	{
	case GIT_DIFF_LINE_ADDITION:
		diffHunk->diffLines.emplace_back(DiffLine::AddedLine(diffResult->current_new_line_index, diffResult->current_old_line_index, line->content, line->content_len));
		diffHunk->rawLines.push_back("+" + std::string(line->content, line->content_len));
		diffResult->current_new_line_index++;
		break;
	case GIT_DIFF_LINE_DELETION:
		diffHunk->diffLines.emplace_back(DiffLine::DeletedLine(diffResult->current_new_line_index, diffResult->current_old_line_index, line->content, line->content_len));
		diffHunk->rawLines.push_back("-" + std::string(line->content, line->content_len));
		diffResult->current_old_line_index++;
		break;
	case GIT_DIFF_LINE_CONTEXT:
		diffHunk->diffLines.emplace_back(DiffLine::ContextLine(diffResult->current_new_line_index, diffResult->current_old_line_index, line->content, line->content_len));
		diffHunk->rawLines.push_back(" " + std::string(line->content, line->content_len));
		diffResult->current_new_line_index++;
		diffResult->current_old_line_index++;
		break;
	default:
		break;
	}
	//std::cout << "git_diff_line_callback " << diffHunk->rawLines.back();
	return 0;
};

void GIT::printDiffResults(std::vector<GIT::DiffResult>& diffResults)
{
	for (auto diffResult : diffResults)
	{
		std::cout << "\nFile: " << diffResult.filePath << std::endl;
		for (auto diffHunk : diffResult.diffHunks)
		{
			for (auto diffLine : diffHunk.diffLines)
			{
				switch (diffLine.type)
				{
				case DiffLine::LINETYPE::ADDED:
					std::cout << "+" + diffLine.line;
					break;
				case DiffLine::LINETYPE::DELETED:
					std::cout << "-" + diffLine.line;
					break;
				case DiffLine::LINETYPE::CONTEXT:
					std::cout << " " + diffLine.line;
					break;
				}
			}
		}

	}
}
std::vector<GIT::DiffResult> GIT::gitDiff() // git diff
{

	git_diff* diff = nullptr;
	git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
	opts.context_lines = 2;
	if (git_diff_index_to_workdir(&diff, repo, nullptr, &opts) < 0)
		getLastError("git_diff_index_to_workdir failed: ");

	std::vector<GIT::DiffResult> diffResults;
	if (git_diff_foreach(diff, git_diff_file_callback, nullptr, git_diff_hunk_callback, git_diff_line_callback, &diffResults) < 0)
		getLastError("git_diff_foreach failed: ");

	git_diff_free(diff);

	return diffResults;
}

std::vector<GIT::DiffResult> GIT::gitDiffHead() // git diff HEAD
{
	git_object* head_tree_obj = nullptr;
	if (git_revparse_single(&head_tree_obj, repo, "HEAD^{tree}") < 0)
		getLastError("git_revparse_single failed: ");

	git_tree* head_tree = (git_tree*)head_tree_obj;

	git_diff* diff = nullptr;
	git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
	opts.context_lines = 2;
	if (git_diff_tree_to_workdir_with_index(&diff, repo, head_tree, &opts) < 0)
		getLastError("git_diff_tree_to_workdir_with_index failed: ");

	std::vector<GIT::DiffResult> diffResults;
	if (git_diff_foreach(diff, git_diff_file_callback, nullptr, git_diff_hunk_callback, git_diff_line_callback, &diffResults) < 0)
		getLastError("git_diff_foreach failed: ");

	git_diff_free(diff);
	git_tree_free(head_tree);

	return diffResults;
}

std::vector<GIT::Commit> GIT::gitLog()
{
	git_revwalk* walker = nullptr;
	if (git_revwalk_new(&walker, repo) < GIT_OK)
		getLastError("Failed to git_revwalk_new: ");
	if (git_revwalk_sorting(walker, GIT_SORT_TIME | GIT_SORT_REVERSE) < GIT_OK)
		getLastError("Failed to git_revwalk_sorting: ");
	if (git_revwalk_push_head(walker) < GIT_OK)
		getLastError("Failed to git_revwalk_push_head: ");
	std::vector<GIT::Commit> commits;
	git_oid oid;
	while (git_revwalk_next(&oid, walker) == GIT_OK)
	{
		git_commit* commit = nullptr;
		if (git_commit_lookup(&commit, repo, &oid) < GIT_OK)
			getLastError("Failed to git_commit_lookup: ");

		const char* message = git_commit_message(commit);
		const git_signature* author = git_commit_author(commit);
		char oid_str[GIT_OID_HEXSZ + 1];
		git_oid_tostr(oid_str, sizeof(oid_str), &oid);

		// store to commit history		
		commits.emplace_back(oid, std::string(oid_str), Author(author->name, author->email, author->when), std::string(message));

		git_commit_free(commit);

	}

	git_revwalk_free(walker);

	return commits;

}

std::string GIT::getContentsAtCommit(std::string filePath, std::string commit_oid_str)
{
	git_oid commit_oid;
	git_commit* commit = nullptr;
	git_tree* tree = nullptr;
	git_tree_entry* entry = nullptr;
	git_blob* blob = nullptr;
	std::string file_content;

	if (git_oid_fromstr(&commit_oid, commit_oid_str.c_str()) < GIT_OK)
		getLastError("Failed to git_oid_fromstr: ");

	if (git_commit_lookup(&commit, repo, &commit_oid) < GIT_OK)
		getLastError("Failed to git_commit_lookup: ");

	if (git_commit_tree(&tree, commit) < GIT_OK)
		getLastError("Failed to git_commit_tree: ");

	if(git_tree_entry_bypath(&entry, tree, filePath.c_str()) < GIT_OK)
		getLastError("Failed to git_tree_entry_bypath: ");

	if (git_blob_lookup(&blob, repo, git_tree_entry_id(entry)) < GIT_OK)
		getLastError("Failed to git_blob_lookup: ");

	file_content = std::string(static_cast<const char*>(git_blob_rawcontent(blob)), git_blob_rawsize(blob));
	
	git_blob_free(blob);
	git_tree_entry_free(entry);
	git_tree_free(tree);
	git_commit_free(commit);

	return file_content;
}

GIT::~GIT()
{
	git_repository_free(repo);
	git_libgit2_shutdown();
}