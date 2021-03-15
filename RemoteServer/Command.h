#pragma once

#include<string>
#include<WinSock2.h>

class Command
{
public:
    virtual void Execute(SOCKET& sock) = 0;
};

Command* GetCommand(const std::string& command); // factory pattern
