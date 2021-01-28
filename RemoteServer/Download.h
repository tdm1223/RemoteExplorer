#ifndef DOWNLOAD_COMMAND
#define DOWNLOAD_COMMAND

#include"Action.h"
#include<iostream>

class Download : public Action
{
public:
    void Execute(SOCKET& sock)
    {
        std::cout << "Download start" << std::endl;
    }
};

#endif DOWNLOAD_COMMAND