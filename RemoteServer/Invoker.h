#pragma once
#include "Packet.h"
#include "Command.h"
#include<map>

class Invoker
{
public:
    enum { UPLOAD = 1, DOWNLOAD = 2, END = 3};
    Invoker();
    void Initialize();
    std::map<int, Command*> GetCommandFactory();

private:
    std::map<int, Command*> commandFactory;
};
