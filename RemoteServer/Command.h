#pragma once

#include<string>
#include<WinSock2.h>

class Command
{
public:
    virtual void Execute(SOCKET& sock) = 0;
    static const int kBufSize = 4096;
    void SerializeInt(const int input, char* output);

};

Command* GetCommand(const std::string& command); // factory pattern
