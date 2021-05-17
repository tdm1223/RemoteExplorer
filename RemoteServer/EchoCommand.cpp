#include"EchoCommand.h"

bool EchoCommand::Execute(SOCKET sock, char* buf)
{
    Sleep(2000);
    if (!Send(sock, buf))
    {
        return false;
    }
    return true;
}