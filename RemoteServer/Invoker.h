#ifndef INVOKER_H
#define INVOKER_H
#include"Command.h"
#include<map>
class Invoker
{
public:
    enum { UPLOAD = 1, DOWNLOAD = 2, MOVE = 3, END = 4 };
    void Initialize();
    std::map<int, Command*> GetCommandFactory();
private:
    std::map<int, Command*> commandFactory;
};
#endif