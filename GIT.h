#pragma once

#include <git2.h>

#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>

#include <windows.h>

#include <vector>
#include <set>

#include<iostream>

#define GIT_TIMEOUT 1


#define U8strToU(str) (reinterpret_cast<const char*>(str.c_str()))
#define U8ToU(str) (reinterpret_cast<const char*>(str))
#define U8strToUstr(str) (std::string(reinterpret_cast<const char*>(str.c_str())))
#define U8ToUstr(str) (std::string(reinterpret_cast<const char*>(str)))
#define UstrToU8(str) (reinterpret_cast<const char8_t*>(str.c_str()))
#define UToU8(str) (reinterpret_cast<const char8_t*>(str))
#define UstrToU8str(str) (std::u8string(reinterpret_cast<const char8_t*>(str.c_str())))
#define UToU8str(str) (std::u8string(reinterpret_cast<const char8_t*>(str)))

class GIT
{
public:
	static std::string localToUtf8(const std::string& localStr);
	static std::u8string u8localToUtf8(const std::string& localStr);

	static std::string utf8ToLocal(const std::string& utf8Str);
	static std::string u8utf8ToLocal(const std::u8string& utf8Str);

	
	struct NotStaged
	{
		std::vector<std::u8string> newFiles;
		std::vector<std::u8string> modifiedFiles;
		std::vector<std::u8string> deletedFiles;
		std::vector<std::u8string> renamedFiles;
		std::vector<std::u8string> typechangedFiles;
		std::vector<std::u8string> unreadableFiles;
	};
	struct Staged
	{
		std::vector<std::u8string> newFiles;
		std::vector<std::u8string> modifiedFiles;
		std::vector<std::u8string> deletedFiles;
		std::vector<std::u8string> renamedFiles;
		std::vector<std::u8string> typechangedFiles;
	};
	struct FileStatus
	{
		NotStaged notStaged;
		Staged staged;
		
	};

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
		std::u8string line;
		DiffLine(int type, int newLineNum, int oldLineNum, std::u8string line) : type(type), newLineNum(newLineNum), oldLineNum(oldLineNum), line(line) {};
		DiffLine(int type, int newLineNum, int oldLineNum, std::string line_u8) : type(type), newLineNum(newLineNum), oldLineNum(oldLineNum), line(UstrToU8str(line_u8)) {};
		DiffLine(int type, int newLineNum, int oldLineNum, const char8_t* content, size_t content_len) : type(type), newLineNum(newLineNum), oldLineNum(oldLineNum), line(std::u8string(content, content_len)) {};
		DiffLine(int type, int newLineNum, int oldLineNum, const char* content_u8, size_t content_len) : type(type), newLineNum(newLineNum), oldLineNum(oldLineNum), line(UstrToU8str(std::string(content_u8, content_len))) {};
		static DiffLine ContextLine(int newLineNum, int oldLineNum, std::u8string line) { return DiffLine(LINETYPE::CONTEXT, newLineNum, oldLineNum, line); };
		static DiffLine ContextLine(int newLineNum, int oldLineNum, std::string line_u8) { return DiffLine(LINETYPE::CONTEXT, newLineNum, oldLineNum, line_u8); };
		static DiffLine ContextLine(int newLineNum, int oldLineNum, const char8_t* content, size_t content_len) { return DiffLine(LINETYPE::CONTEXT, newLineNum, oldLineNum, content, content_len); };
		static DiffLine ContextLine(int newLineNum, int oldLineNum, const char* content_u8, size_t content_len) { return DiffLine(LINETYPE::CONTEXT, newLineNum, oldLineNum, content_u8, content_len); };
		static DiffLine AddedLine(int newLineNum, int oldLineNum, std::u8string line) { return DiffLine(LINETYPE::ADDED, newLineNum, oldLineNum, line); };
		static DiffLine AddedLine(int newLineNum, int oldLineNum, std::string line_u8) { return DiffLine(LINETYPE::ADDED, newLineNum, oldLineNum, line_u8); };
		static DiffLine AddedLine(int newLineNum, int oldLineNum, const char8_t* content, size_t content_len) { return DiffLine(LINETYPE::ADDED, newLineNum, oldLineNum, content, content_len); };
		static DiffLine AddedLine(int newLineNum, int oldLineNum, const char* content_u8, size_t content_len) { return DiffLine(LINETYPE::ADDED, newLineNum, oldLineNum, content_u8, content_len); };
		static DiffLine DeletedLine(int newLineNum, int oldLineNum, std::u8string line) { return DiffLine(LINETYPE::DELETED, newLineNum, oldLineNum, line); };
		static DiffLine DeletedLine(int newLineNum, int oldLineNum, std::string line_u8) { return DiffLine(LINETYPE::DELETED, newLineNum, oldLineNum, line_u8); };
		static DiffLine DeletedLine(int newLineNum, int oldLineNum, const char8_t* content, size_t content_len) { return DiffLine(LINETYPE::DELETED, newLineNum, oldLineNum, content, content_len); };
		static DiffLine DeletedLine(int newLineNum, int oldLineNum, const char* content_u8, size_t content_len) { return DiffLine(LINETYPE::DELETED, newLineNum, oldLineNum, content_u8, content_len); };
	};

	struct DiffHunk
	{
		std::vector<DiffLine> diffLines;
		std::vector<std::u8string> rawLines;
		git_diff_hunk hunk;
	};
	struct DiffResult
	{
		std::u8string filePath;
		std::vector<DiffHunk> diffHunks;
		int current_new_line_index = -1;
		int current_old_line_index = -1;
	};
	
	struct Author
	{
		std::u8string name;
		std::u8string email;
		git_time when;
		Author(std::u8string name, std::u8string email, git_time when) : name(name), email(email), when(when) {};
		Author(std::string name_u8, std::string email_u8, git_time when) : name(UstrToU8str(name_u8)), email(UstrToU8str(email_u8)), when(when) {};
	};
	struct Commit
	{

		const git_oid oid;
		const std::u8string oid_str;
		const Author author;
		const std::u8string message;
		Commit(git_oid oid, std::u8string oid_str, Author author, std::u8string message) : oid(oid), oid_str(oid_str), author(author), message(message) {};
		Commit(git_oid oid, std::string oid_str_u8, Author author, std::string message_u8) : oid(oid), oid_str(UstrToU8str(oid_str_u8)), author(author), message(UstrToU8str(message_u8)) {};

	};

	static git_diff_file_cb git_diff_file_callback;
	static git_diff_hunk_cb git_diff_hunk_callback;
	static git_diff_line_cb git_diff_line_callback;

private:
	std::u8string repoPath;
	git_repository* repo = nullptr;
	std::u8string userName = u8"";
	std::u8string userEmail = u8"";


	void printErrorAndShutdown(std::u8string text=u8"");
	void printErrorAndShutdown(std::string text_local8bit = "") { printErrorAndShutdown(u8localToUtf8(text_local8bit)); };
	
	void getLastError(std::u8string info = u8"");
	void getLastError(std::string info_local8bit = "") { getLastError(u8localToUtf8(info_local8bit)); };


	std::vector<DiffResult> u8gitDiffHeadToMemory(std::u8string filePath, std::u8string memory);

	FileStatus collectRepoStatus();	

	std::vector<std::string> ignorePreset = { ".vs", "x64" };

	std::u8string u8printRepoStatus(const FileStatus& fileStatus);



	
	
	void u8gitAdd(std::u8string filePath) { return stagingFiles({ U8strToUstr(filePath) }); };
	void u8gitCommit(std::u8string message);

	std::u8string u8getContentsAtCommit(std::string filePath_u8, std::string commit_oid_str_u8);
	std::u8string u8getContentsAtCommit(std::u8string filePath, std::u8string commit_oid_str) { return u8getContentsAtCommit(U8strToUstr(filePath), U8strToUstr(commit_oid_str)); };	
	std::u8string u8getContentsAtBranch(std::string filePath_u8, std::string branch_name_u8);
	std::u8string u8getContentsAtBranch(std::u8string filePath, std::u8string branch_name) { return u8getContentsAtBranch(U8strToUstr(filePath), U8strToUstr(branch_name)); };

	void u8createBranch(const std::u8string& branch_name);
	void u8switchBranch(const std::u8string& branch_name);
	void u8mergeBranch(const std::u8string& source_branch);
	void u8deleteBranch(const std::u8string& branch_name);


public:			
	GIT(std::u8string repoPath, std::u8string userName = u8"", std::u8string userEmail = u8"");
	GIT(std::u8string repoPath, std::pair<std::u8string, std::u8string> user)
		:GIT(repoPath, user.first, user.second) {};
	GIT(std::string repoPath_local8bit, std::string userName_local8bit = "", std::string userEmail_local8bit = "")
		: GIT(u8localToUtf8(repoPath_local8bit), u8localToUtf8(userName_local8bit), u8localToUtf8(userEmail_local8bit)) {};
	GIT(std::string repoPath_local8bit, std::pair<std::string, std::string> user_local8bit)
		: GIT(u8localToUtf8(repoPath_local8bit), u8localToUtf8(user_local8bit.first), u8localToUtf8(user_local8bit.second)) {};

	static bool isRepoExist(std::u8string repoPath); 
	static bool isRepoExist(std::string repoPath_local8bit)
	{ return isRepoExist(u8localToUtf8(repoPath_local8bit)); };


	static GIT* cloneFromRemote(std::u8string remoteRepoPath, std::u8string localRepoPath, std::u8string userName = u8"", std::u8string userEmail = u8"");
	static GIT* cloneFromRemote(std::u8string remoteRepoPath, std::u8string localRepoPath, std::pair<std::u8string, std::u8string> user) 
	{ return cloneFromRemote(remoteRepoPath, localRepoPath, user.first, user.second); };
	static GIT* cloneFromRemote(std::string remoteRepoPath_local8bit, std::string localRepoPath_local8bit, std::string userName_local8bit = "", std::string userEmail_local8bit = "")
	{ return cloneFromRemote(u8localToUtf8(remoteRepoPath_local8bit), u8localToUtf8(localRepoPath_local8bit), u8localToUtf8(userName_local8bit), u8localToUtf8(userEmail_local8bit)); };
	static GIT* cloneFromRemote(std::string remoteRepoPath_local8bit, std::string localRepoPath_local8bit, std::pair<std::string, std::string> user_local8bit)
	{ return cloneFromRemote(remoteRepoPath_local8bit, localRepoPath_local8bit, user_local8bit.first, user_local8bit.second); };

	void clearGitIgnore();
	void appendGitIgnore(const std::vector<std::u8string>& ignorePatterns);
	void appendGitIgnore(const std::vector<std::string>& ignorePatterns_local8bit);
	
	void stagingAll();
	void stagingFiles(std::vector<std::u8string> filesPath);
	void stagingFiles(std::vector<std::string> filesPath_local8bit);
	void stagingAllUntrackedFiles();
	void stagingAllModifiedFiles();
	void stagingAllDeletedFiles();
	void stagingAllRenamedFiles();
	void stagingAllTypechangedFiles();
		
	void printDiffResults(std::vector<DiffResult>& diffResults);
	std::vector<DiffResult> gitDiff();
	std::vector<DiffResult> gitDiffHead();	
	std::vector<DiffResult> gitDiffHeadToMemory(std::string filePath_local8bit, std::string memory_lcoal8bit) { return u8gitDiffHeadToMemory(u8localToUtf8(filePath_local8bit), u8localToUtf8(memory_lcoal8bit)); };
	std::vector<Commit> gitLog();
	

	void gitAdd(std::string filePath_local8bit) { return u8gitAdd(u8localToUtf8(filePath_local8bit)); };

	void gitCommit(std::string message_local8bit) { return u8gitCommit(u8localToUtf8(message_local8bit)); };

	void gitPull();
	void gitPush();

	std::string getContentsAtCommit(std::string filePath_local8bit, std::string commit_oid_str_local8bit) { return u8utf8ToLocal(u8getContentsAtCommit(u8localToUtf8(filePath_local8bit), u8localToUtf8(commit_oid_str_local8bit))); };	
	std::string getContentsAtBranch(std::string filePath_local8bit, std::string branch_name_local8bit) { return u8utf8ToLocal(u8getContentsAtBranch(u8localToUtf8(filePath_local8bit), u8localToUtf8(branch_name_local8bit))); };

	

	std::string printRepoStatus(const FileStatus& fileStatus) { return u8utf8ToLocal(u8printRepoStatus(fileStatus)); };

	std::string getCurrentBranch();
	
	std::string getCurrentStatus();


	void createBranch(const std::string& branch_name_local8bit) { return u8createBranch(u8localToUtf8(branch_name_local8bit)); };
	void switchBranch(const std::string& branch_name_local8bit) { return u8switchBranch(u8localToUtf8(branch_name_local8bit)); };
	void mergeBranch(const std::string& source_branch_local8bit) { return u8mergeBranch(u8localToUtf8(source_branch_local8bit)); };
	void deleteBranch(const std::string& branch_name_local8bit) { return u8deleteBranch(u8localToUtf8(branch_name_local8bit)); };

	std::vector<std::string> getBranchList(git_branch_t branch_type_enum);
	std::vector<std::string> getLocalBranchList();
	std::vector<std::string> getRemoteBranchList();
	std::vector<std::string> getAllBranchList();	
	

	~GIT();


};

