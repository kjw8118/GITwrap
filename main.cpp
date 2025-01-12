
#include "GIT.h"


#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std::string_literals;

bool testForRemoteClone()
{
    auto localRepoPath = std::string("C:\\Users\\Jinwon\\source\\repos\\GIT\\repoEx\\로컬\\리포1");
    auto remoteRepoPath = std::string("C:\\Users\\Jinwon\\source\\repos\\GIT\\repoEx\\리모트\\리포1");
    GIT* remoteGit = nullptr; GIT* localGit = nullptr; GIT* copyGit = nullptr;
    if (!GIT::isRepoExist(remoteRepoPath))
        return false;
    remoteGit = new GIT(remoteRepoPath, "Jinwon", "kjw8118@gmail.com");

    if (!GIT::isRepoExist(localRepoPath))
    {
        localGit = GIT::cloneFromRemote(remoteRepoPath, localRepoPath, "Jinwon", "kjw8118@gmail.com");
    }
    else
    {
        localGit = new GIT(localRepoPath, "Jinwon", "kjw8118@gmail.com");
        //git->check_current_tip_and_first_parent();
        //return 0;
        auto fileName = std::string("test.txt");
        auto filePath = localRepoPath + "\\" + fileName;
        //if (!std::filesystem::exists(filePath))        
        std::ofstream file(filePath, std::ios::app);
        file << "new line" << std::endl;
        file.close();
        //git->gitAdd(fileName);
        localGit->stagingAll();
        localGit->gitCommit("test edit");
        //auto branches = git->getAllBranchList();
        //for (auto branch : branches)
        //    std::cout << branch << std::endl;
        localGit->gitPush();
    }



    if (remoteGit && false)
    {
        auto remoteLog = remoteGit->gitLog();
        std::cout << "Remote Repo git log: " << std::endl;
        for (auto log : remoteLog)
            std::cout << "\t" << (log.oid_str) << std::endl;
    }
    if (localGit && false)
    {
        auto localLog = localGit->gitLog();
        std::cout << "Local Repo git log: " << std::endl;
        for (auto log : localLog)
            std::cout << "\t" << (log.oid_str) << std::endl;
    }
    auto copyRepoPath = std::string("C:\\Users\\Jinwon\\source\\repos\\GIT\\repoEx\\카피\\리포1");
    //if (!GIT::isRepoExist(copyRepoPath))
    //    copyGit = GIT::cloneFromRemote(remoteRepoPath, copyRepoPath, "Jinwon", "kjw8118@gmail.com");
    //else
    copyGit = new GIT(copyRepoPath, "Jinwon", "kjw8118@gmail.com");
    if (copyGit)
    {
        copyGit->gitPull();
        auto copyLog = copyGit->gitLog();
        std::cout << "Copy Repo git log\t: ";
        if (!copyLog.empty())
            std::cout << (copyLog.back().oid_str) << std::endl;
    }

    return true;
}

int main()
{
    std::cout << "Hello World!\n";

    auto git = new GIT("C:\\Users\\Jinwon\\repo3");
    auto branchList = git->getAllBranchList();
    for (auto& branch : branchList)
        std::cout << branch << std::endl;

    auto commitList = git->gitLog();
    for (auto& commit : commitList)
        std::cout << commit.message << std::endl;
    /*
    std::string fPath = "repoEx\\리포\\한글문서.txt";
    std::fstream file(fPath.c_str());
    bool isOpen = file.is_open();
    std::ostringstream ss;
    ss << file.rdbuf();


    auto git = new GIT("repoEx\\리포\\", "Jinwon", "kjw8118@gmail.com");
    //git->gitAdd("한글문서.txt");
    //git->gitCommit("한글 커밋 메시지 입니다.");
    auto commitList = git->gitLog();
    for (auto commit : commitList)
        std::cout << commit.message << std::endl;
    auto contents = git->gitShowFromCommit("한글문서.txt", commitList.back().oid_str);
    auto contents0 = ss.str();
    std::cout << GIT::utf8ToEucKrAndLatin1(contents) << std::endl;
    std::cout << GIT::utf8ToEucKrAndLatin1(contents0) << std::endl;
    
    return 0;*/
};
/*

    char mix[] = { 0xc3, 0xbc, 0xED, 0x95, 0x9C, 0xEA, 0xB8, 0x80, 0x20, 0x00 };
    char newMix[] = { 0xFC, 0xC7, 0xD1, 0xB1, 0xDB, 0x00 };

    SetConsoleOutputCP(CP_UTF8);
    std::string mix_str(mix);
    std::cout << mix_str << std::endl;*/
    /*for (unsigned char c : mix_str)
        std::cout << (int)c << ", ";
    std::cout << std::endl;

    SetConsoleOutputCP(51949);
    auto mix_str_euckr = GIT::utf8ToLocal(mix_str);
    std::cout << mix_str_euckr << std::endl;
    for (unsigned char c : mix_str_euckr)
        std::cout << (int)c << ", ";
    std::cout << std::endl;

    SetConsoleOutputCP(28591);
    auto mix_str_latin1 = GIT::utf8ToLatin1(mix_str);
    std::cout << mix_str_latin1 << std::endl;
    for (unsigned char c : mix_str_latin1)
        std::cout << (int)c << ", ";
    std::cout << std::endl;*/

    /*SetConsoleOutputCP(CP_UTF8);
    std::string newMix_str(newMix);
    std::cout << newMix_str << std::endl;
    for (unsigned char c : newMix_str)
        std::cout << std::hex << (int)c << ", ";
    std::cout << std::endl;

    auto newMix_str_utf8 = GIT::mixedToUtf8(newMix);
    std::cout << newMix_str_utf8 << std::endl;
    for (unsigned char c : newMix_str_utf8)
        std::cout << std::hex << (int)c << ", ";
    std::cout << std::endl;

    auto newMix_str_euckr = GIT::utf8ToEucKrAndLatin1(newMix_str_utf8);
    std::cout << newMix_str_euckr << std::endl;
    for (unsigned char c : newMix_str_euckr)
        std::cout << std::hex << (int)c << ", ";
    std::cout << std::endl;

    return 0;





    testForRemoteClone();
    //auto git = new GIT("./repo1", "Jinwon", "kjw8118@gmail.com");
    //auto git1 = new GIT(u8"리포", u8"Jinwon", u8"kjw8118@gmail.com");
    //auto git2 = new GIT("리포", "Jinwon", "kjw8118@gmail.com");
    
    auto repoPath = std::string("C:\\Users\\Jinwon\\source\\repos\\GIT\\repoEx\\diffMem");
    auto git = new GIT(repoPath);
    auto diffResults = git->gitDiffHeadToMemory("test.txt", "헬로월드\n");
    git->printDiffResults(diffResults);
    
    //auto git = new GIT("./repo", "Jinwon", "kjw8118@gmail.com");
    //auto git = new GIT("./repo", "Jinwon", "kjw8118@gmail.com");
    //git->stagingAll();
    //std::cout << "Current status\n" << git->getCurrentStatus() << std::endl;
    //git->commitCurrentStage("new");
    //std::cout << "Current branch: " << git->getCurrentBranch() << std::endl;
    //git->compare();
    //git->gitLog();
    
    
}*/

