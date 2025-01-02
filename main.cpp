
#include "GIT.h"


#include <iostream>

using namespace std::string_literals;

int main()
{
    std::cout << "Hello World!\n";

    //auto git = new GIT("./repo1", "Jinwon", "kjw8118@gmail.com");
    //auto git1 = new GIT(u8"리포", u8"Jinwon", u8"kjw8118@gmail.com");
    auto git2 = new GIT("리포", "Jinwon", "kjw8118@gmail.com");
    
    //auto git = new GIT("./repo", "Jinwon", "kjw8118@gmail.com");
    //auto git = new GIT("./repo", "Jinwon", "kjw8118@gmail.com");
    //git->stagingAll();
    //std::cout << "Current status\n" << git->getCurrentStatus() << std::endl;
    //git->commitCurrentStage("new");
    //std::cout << "Current branch: " << git->getCurrentBranch() << std::endl;
    //git->compare();
    //git->gitLog();
    

}

