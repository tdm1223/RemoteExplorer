#ifndef UPLOAD_COMMAND
#define UPLOAD_COMMAND

#include"Command.h"
#include<iostream>

class Upload : public Command
{
public:
    void Execute(SOCKET& sock, std::string& name);
};

#endif UPLOAD_COMMAND