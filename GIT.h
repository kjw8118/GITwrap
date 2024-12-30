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
	
	void commitCurrentStage(std::string commit_message);

	std::string getCurrentBranch();

	std::string getCurrentStatus();


	void createBranch(const std::string& branch_name);
	void switchBranch(const std::string& branch_name);
	void mergeBranch(const std::string& source_branch);
	void deleteBranch(const std::string& branch_name);


	~GIT();


};

