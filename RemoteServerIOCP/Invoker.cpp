#include"Invoker.h"
#include"Util.h"

void Invoker::Initialize()
{
    commandFactory[Util::ECHO] = GetCommand("ECHO");
    commandFactory[Util::DOWNLOAD] = GetCommand("DOWNLOAD");
    commandFactory[Util::LIST] = GetCommand("LIST");
    commandFactory[Util::END] = GetCommand("END");
}
std::map<int, Command*> Invoker::GetCommandFactory()
{
    return commandFactory;
}