#ifndef SERVER_ECHO_COMMAND_H
#define SERVER_ECHO_COMMAND_H

#include"Command.h"

class EchoCommand : public Command
{
public:
    bool Execute(SOCKET sock, char* buf);
};

#endif // _DEBUG