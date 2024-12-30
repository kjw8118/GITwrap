
#include "GIT.h"


#include <iostream>


int main()
{
    std::cout << "Hello World!\n";

    auto git = new GIT("./repo");
    git->stagingAll();
    std::cout << "Current status\n" << git->getCurrentStatus() << std::endl;
    //std::cout << "Current branch: " << git->getCurrentBranch() << std::endl;


}

