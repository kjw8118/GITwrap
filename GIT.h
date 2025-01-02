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

private:
	std::u8string repoPath;
	git_repository* repo = nullptr;
	std::u8string userName = u8"";
	std::u8string userEmail = u8"";

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

	void printErrorAndShutdown(std::u8string text=u8"");
	void printErrorAndShutdown(std::string text_local = "") { printErrorAndShutdown(u8localToUtf8(text_local)); };
	
	void getLastError(std::u8string info = u8"");
	void getLastError(std::string info_local = "") { getLastError(u8localToUtf8(info_local)); };

	
	FileStatus collectRepoStatus();	

	std::vector<std::string> ignorePreset = { ".vs", "x64" };

	std::u8string u8printRepoStatus(const FileStatus& fileStatus);



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
	static git_diff_file_cb git_diff_file_callback;
	static git_diff_hunk_cb git_diff_hunk_callback;
	static git_diff_line_cb git_diff_line_callback;

	
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
	
		
	void u8commitCurrentStage(std::u8string commit_message);

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
	GIT(std::string repoPath_local, std::string userName_local = "", std::string userEmail_local = "") : GIT(u8localToUtf8(repoPath_local), u8localToUtf8(userName_local), u8localToUtf8(userEmail_local)) {};
	static bool isRepoExist(std::u8string repoPath); 
	static bool isRepoExist(std::string repoPath_local) { return isRepoExist(u8localToUtf8(repoPath_local)); };

	void clearGitIgnore();
	void appendGitIgnore(const std::vector<std::u8string>& ignorePatterns);
	void appendGitIgnore(const std::vector<std::string>& ignorePatterns_local);
	
	void stagingAll();
	void stagingFiles(std::vector<std::u8string> filesPath);
	void stagingFiles(std::vector<std::string> filesPath_local);
	void stagingAllUntrackedFiles();
	void stagingAllModifiedFiles();
	void stagingAllDeletedFiles();
	void stagingAllRenamedFiles();
	void stagingAllTypechangedFiles();
		
	void printDiffResults(std::vector<DiffResult>& diffResults);
	std::vector<DiffResult> gitDiff();
	std::vector<DiffResult> gitDiffHead();
	std::vector<Commit> gitLog();
	
	std::string getContentsAtCommit(std::string filePath_local, std::string commit_oid_str_local) { return u8utf8ToLocal(u8getContentsAtCommit(u8localToUtf8(filePath_local), u8localToUtf8(commit_oid_str_local))); };	
	std::string getContentsAtBranch(std::string filePath_local, std::string branch_name_local) { return u8utf8ToLocal(u8getContentsAtBranch(u8localToUtf8(filePath_local), u8localToUtf8(branch_name_local))); };

	void commitCurrentStage(std::string commit_message_local) { return u8commitCurrentStage(u8localToUtf8(commit_message_local)); };

	std::string printRepoStatus(const FileStatus& fileStatus) { return u8utf8ToLocal(u8printRepoStatus(fileStatus)); };

	std::string getCurrentBranch();
	
	std::string getCurrentStatus();


	void createBranch(const std::string& branch_name_local) { return u8createBranch(u8localToUtf8(branch_name_local)); };
	void switchBranch(const std::string& branch_name_local) { return u8switchBranch(u8localToUtf8(branch_name_local)); };
	void mergeBranch(const std::string& source_branch_local) { return u8mergeBranch(u8localToUtf8(source_branch_local)); };
	void deleteBranch(const std::string& branch_name_local) { return u8deleteBranch(u8localToUtf8(branch_name_local)); };

	std::vector<std::string> getBranchList(git_branch_t branch_type_enum);
	std::vector<std::string> getLocalBranchList();
	std::vector<std::string> getRemoteBranchList();
	std::vector<std::string> getAllBranchList();

	
	
	

	~GIT();


};

