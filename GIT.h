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

	class DiffLine
	{
	public:
		enum LINETYPE
		{
			CONTEXT = 0,
			ADDED = 1,
			DELETED,
		};
		int type;
		int newLineNum;
		int oldLineNum;
		std::string line;
		DiffLine(int type, int newLineNum, int oldLineNum, std::string line)
			: type(type), newLineNum(newLineNum), oldLineNum(oldLineNum), line(line) {};
		DiffLine(int type, int newLineNum, int oldLineNum, const char* content, size_t content_len)
			: type(type), newLineNum(newLineNum), oldLineNum(oldLineNum), line(std::string(content, content_len)) {};
		static DiffLine ContextLine(int newLineNum, int oldLineNum, std::string line) { return DiffLine(LINETYPE::CONTEXT, newLineNum, oldLineNum, line); };
		static DiffLine ContextLine(int newLineNum, int oldLineNum, const char* content, size_t content_len) { return DiffLine(LINETYPE::CONTEXT, newLineNum, oldLineNum, content, content_len); };
		static DiffLine AddedLine(int newLineNum, int oldLineNum, std::string line) { return DiffLine(LINETYPE::ADDED, newLineNum, oldLineNum, line); };
		static DiffLine AddedLine(int newLineNum, int oldLineNum, const char* content, size_t content_len) { return DiffLine(LINETYPE::ADDED, newLineNum, oldLineNum, content, content_len); };
		static DiffLine DeletedLine(int newLineNum, int oldLineNum, std::string line) { return DiffLine(LINETYPE::DELETED, newLineNum, oldLineNum, line); };
		static DiffLine DeletedLine(int newLineNum, int oldLineNum, const char* content, size_t content_len) { return DiffLine(LINETYPE::DELETED, newLineNum, oldLineNum, content, content_len); };
	};

	struct DiffHunk
	{
		std::vector<DiffLine> diffLines;
		std::vector<std::string> rawLines;
		git_diff_hunk hunk;
	};
	struct DiffResult
	{
		std::string filePath;
		std::vector<DiffHunk> diffHunks;
		int current_new_line_index = -1;
		int current_old_line_index = -1;
	};
	static git_diff_file_cb git_diff_file_callback;
	static git_diff_hunk_cb git_diff_hunk_callback;
	static git_diff_line_cb git_diff_line_callback;

	void printDiffResults(std::vector<DiffResult>& diffResults);
	std::vector<DiffResult> gitDiff(); // git diff	
	std::vector<DiffResult> gitDiffHead(); // git diff HEAD
	
	
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
		gitDiff();
		std::cout << "\n\ncompare_head_to_workdir" << std::endl;
		gitDiffHead(); 
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

