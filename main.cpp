
#include "GIT.h"


#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std::string_literals;

int main()
{
    std::cout << "Hello World!\n";

    //auto git = new GIT("./repo1", "Jinwon", "kjw8118@gmail.com");
    //auto git1 = new GIT(u8"리포", u8"Jinwon", u8"kjw8118@gmail.com");
    //auto git2 = new GIT("리포", "Jinwon", "kjw8118@gmail.com");
    
    
    auto localRepoPath = std::string("C:\\Users\\Jinwon\\source\\repos\\GIT\\repoEx\\로컬\\리포1");
    auto remoteRepoPath = std::string("C:\\Users\\Jinwon\\source\\repos\\GIT\\repoEx\\리모트\\리포1");
    GIT* remoteGit = nullptr; GIT* localGit = nullptr; GIT* copyGit = nullptr;
    if (!GIT::isRepoExist(remoteRepoPath))
        return -1;
    remoteGit = new GIT(remoteRepoPath, "Jinwon", "kjw8118@gmail.com");

    if(!GIT::isRepoExist(localRepoPath))
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
        for(auto log: remoteLog)
            std::cout << "\t" << GIT::u8utf8ToLocal(log.oid_str) << std::endl;
    }
    if (localGit && false)
    {
        auto localLog = localGit->gitLog();
        std::cout << "Local Repo git log: " << std::endl;
        for (auto log : localLog)
            std::cout << "\t" << GIT::u8utf8ToLocal(log.oid_str) << std::endl;
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
        if(!copyLog.empty())
            std::cout << GIT::u8utf8ToLocal(copyLog.back().oid_str) << std::endl;
    }
    
    
    //auto git = new GIT("./repo", "Jinwon", "kjw8118@gmail.com");
    //auto git = new GIT("./repo", "Jinwon", "kjw8118@gmail.com");
    //git->stagingAll();
    //std::cout << "Current status\n" << git->getCurrentStatus() << std::endl;
    //git->commitCurrentStage("new");
    //std::cout << "Current branch: " << git->getCurrentBranch() << std::endl;
    //git->compare();
    //git->gitLog();
    

}

