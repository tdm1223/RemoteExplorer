#ifndef UPLOAD_COMMAND
#define UPLOAD_COMMAND

#include"Action.h"
#include<iostream>

class Upload : public Action
{
public:
    void Execute(SOCKET& sock)
    {
        std::cout << "Upload start" << std::endl;
    }
};

#endif UPLOAD_COMMAND