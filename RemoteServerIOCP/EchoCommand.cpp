#include"EchoCommand.h"

bool EchoCommand::Execute(SOCKET sock, char* buf)
{
    std::cout << "Upload" << std::endl;
    return true;
}