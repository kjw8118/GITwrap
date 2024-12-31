#pragma once

#include <git2.h>

#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>

#include <vector>
#include <set>

#include<iostream>

#define GIT_TIMEOUT 1

class GIT
{
private:
	std::string repoPath;
	git_repository* repo = nullptr;
	std::string userName = "";
	std::string userEmail = "";

	struct NotStaged
	{
		std::vector<std::string> newFiles;
		std::vector<std::string> modifiedFiles;
		std::vector<std::string> deletedFiles;
		std::vector<std::string> renamedFiles;
		std::vector<std::string> typechangedFiles;
		std::vector<std::string> unreadableFiles;
	};
	struct Staged
	{
		std::vector<std::string> newFiles;
		std::vector<std::string> modifiedFiles;
		std::vector<std::string> deletedFiles;
		std::vector<std::string> renamedFiles;
		std::vector<std::string> typechangedFiles;
	};
	struct FileStatus
	{
		NotStaged notStaged;
		Staged staged;
		
	};

	void printErrorAndShutdown(std::string text);

	void getLastError(std::string info = "");

	
	FileStatus collectRepoStatus();	

	std::vector<std::string> ignorePreset = { ".vs", "x64" };

	std::string printRepoStatus(const FileStatus& fileStatus);

	struct DiffResult
	{
		std::string filePath;
		std::vector<std::string> addedLines;
		std::vector<std::string> deletedLines;
		std::vector<std::string> contextLines;
	};
	git_diff_file_cb file_callback = [](const git_diff_delta* delta, float progress, void* payload)->int {
		auto diffResults = (std::vector<DiffResult>*)payload;

		diffResults->push_back({ std::string(delta->new_file.path), std::vector<std::string>(), std::vector<std::string>(), std::vector<std::string>() });

		return 0;
	};
	git_diff_line_cb git_diff_line_callback = [](const git_diff_delta* delta, const git_diff_hunk* hunk, const git_diff_line* line, void* payload)->int {
		auto diffResults = (std::vector<DiffResult>*)payload;
		
		DiffResult* diffResult = nullptr;

		if (diffResults->empty())
		{
			diffResults->push_back({ std::string(delta->new_file.path), std::vector<std::string>(), std::vector<std::string>(), std::vector<std::string>() });
			diffResult = &diffResults->back();
		}
		else
		{
			diffResult = &diffResults->back();
			if (diffResult->filePath != std::string(delta->new_file.path))
			{
				diffResults->push_back({ std::string(delta->new_file.path), std::vector<std::string>(), std::vector<std::string>(), std::vector<std::string>() });
				diffResult = &diffResults->back();
			}

		}
			
		switch (line->origin) 
		{
		case GIT_DIFF_LINE_ADDITION:
			diffResult->addedLines.emplace_back(line->content, line->content_len);
			break;
		case GIT_DIFF_LINE_DELETION:
			diffResult->deletedLines.emplace_back(line->content, line->content_len);
			break;
		case GIT_DIFF_LINE_CONTEXT:
			diffResult->contextLines.emplace_back(line->content, line->content_len);
			break;
		default:
			break;
		}

		return 0;
	};

	void compare_workdir_to_index() // git diff
	{

		git_diff* diff = nullptr;
		if(git_diff_index_to_workdir(&diff, repo, nullptr, nullptr) < 0)
			getLastError("git_diff_index_to_workdir failed: ");
		
		std::vector<DiffResult> diffResults;
		if (git_diff_foreach(diff, file_callback, nullptr, nullptr, git_diff_line_callback, &diffResults) < 0)
			getLastError("git_diff_foreach failed: ");
		
		for (auto diffResult : diffResults)
		{
			std::cout << "\nFile: " << diffResult.filePath << std::endl;
			for (auto addLine : diffResult.addedLines)
				std::cout << " + " << addLine;
			for (auto deletedLine : diffResult.deletedLines)
				std::cout << " - " << deletedLine;
			for (auto contextLine : diffResult.contextLines)
				std::cout << " " << contextLine;
		}
		git_diff_free(diff);
	}

	void compare_head_to_workdir()
	{				
		git_object* head_tree_obj = nullptr;
		if (git_revparse_single(&head_tree_obj, repo, "HEAD^{tree}") < 0)
			getLastError("git_revparse_single failed: ");

		git_tree* head_tree = (git_tree*)head_tree_obj;
		
		git_diff* diff = nullptr;
		if (git_diff_tree_to_workdir_with_index(&diff, repo, head_tree, nullptr) < 0)
			getLastError("git_diff_tree_to_workdir_with_index failed: ");

		std::vector<DiffResult> diffResults;
		if (git_diff_foreach(diff, file_callback, nullptr, nullptr, git_diff_line_callback, &diffResults) < 0)
			getLastError("git_diff_foreach failed: ");

		for (auto diffResult : diffResults)
		{
			std::cout << "\nFile: " << diffResult.filePath << std::endl;
			for (auto addLine : diffResult.addedLines)
				std::cout << " + " << addLine;
			for (auto deletedLine : diffResult.deletedLines)
				std::cout << " - " << deletedLine;
			for (auto contextLine : diffResult.contextLines)
				std::cout << " " << contextLine;
		}

		git_diff_free(diff);
		git_tree_free(head_tree);
	}
	
public:
	GIT(std::string repoPath, std::string userName = "", std::string userEmail = "");
	static bool isRepoExist(std::string repoPath);
	
	void clearGitIgnore();
	void appendGitIgnore(const std::vector<std::string>& ignorePatterns);
	
	void stagingAll();
	void stagingFiles(std::vector<std::string> filesPath);
	void stagingAllUntrackedFiles();
	void stagingAllModifiedFiles();
	void stagingAllDeletedFiles();
	void stagingAllRenamedFiles();
	void stagingAllTypechangedFiles();
	
	void compare() {
		std::cout << "\n\ncompare_workdir_to_index" << std::endl;
		compare_workdir_to_index();
		std::cout << "\n\ncompare_head_to_workdir" << std::endl;
		compare_head_to_workdir(); 
	};

	void commitCurrentStage(std::string commit_message);

	std::string getCurrentBranch();

	std::string getCurrentStatus();


	void createBranch(const std::string& branch_name);
	void switchBranch(const std::string& branch_name);
	void mergeBranch(const std::string& source_branch);
	void deleteBranch(const std::string& branch_name);

	std::vector<std::string> getBranchList(git_branch_t branch_type_enum);
	std::vector<std::string> getLocalBranchList();
	std::vector<std::string> getRemoteBranchList();
	std::vector<std::string> getAllBranchList();


	~GIT();


};

