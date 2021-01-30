#ifndef DOWNLOAD_COMMAND
#define DOWNLOAD_COMMAND

#include"Command.h"
#include<iostream>

class Download : public Command
{
public:
    void Execute(SOCKET& sock, std::string& name)
    {
        std::cout << "Download start" << std::endl;
    }
};

#endif DOWNLOAD_COMMAND