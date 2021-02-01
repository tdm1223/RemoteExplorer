﻿#ifndef COMMAND
#define COMMAND
#include<string>
#include"CustomPacket.h"
#include<thread>
class Command
{
public:
    virtual void Execute(SOCKET& sock, std::string& name) = 0;
    CustomPacket packet;
    const int bufSize = 4096;
};

Command* GetCommand(const std::string& command); // factory pattern

#endif