#include"Invoker.h"

void Invoker::Initialize()
{
    commandFactory[UPLOAD] = GetCommand("UPLOAD");
    commandFactory[DOWNLOAD] = GetCommand("DOWNLOAD");
    commandFactory[END] = GetCommand("END");
}

std::map<int, Command*> Invoker::GetCommandFactory()
{
    return commandFactory;
}
