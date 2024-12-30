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

	struct FileStatus
	{
		std::vector<std::string> untracked;
		std::vector<std::string> modified;
		std::vector<std::string> deleted;
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
	
	void commitCurrentStage(std::string commit_message);

	~GIT();


};

