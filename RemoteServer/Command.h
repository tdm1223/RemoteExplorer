#ifndef SERVER_COMMAND_H
#define SERVER_COMMAND_H

#include<string>
#include<WinSock2.h>

class Command
{
public:
    virtual bool Execute(SOCKET& sock) = 0;
    void SerializeInt(const int input, char* output);
    bool SendLength(SOCKET& sock, int length);
    bool Send(SOCKET& sock, char* message);
};

Command* GetCommand(const std::string& command); // factory pattern

#endif // SERVER_COMMAND_H