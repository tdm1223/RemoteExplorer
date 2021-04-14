#ifndef INVOKER_H
#define INVOKER_H

#include"Command.h"
#include<map>

class Invoker
{
public:
    void Initialize();
    std::map<int, Command*> GetCommandFactory();
private:
    std::map<int, Command*> commandFactory;
};
#endif