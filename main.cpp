
#include "GIT.h"


#include <iostream>


int main()
{
    std::cout << "Hello World!\n";

    auto git = new GIT("./repo", "Jinwon", "kjw8118@gmail.com");
    //git->stagingAll();
    std::cout << "Current status\n" << git->getCurrentStatus() << std::endl;
    //git->commitCurrentStage("new");
    //std::cout << "Current branch: " << git->getCurrentBranch() << std::endl;
    git->compare();
    

}

