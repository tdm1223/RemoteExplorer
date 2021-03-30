#ifndef SERVER_COMMAND_H
#define SERVER_COMMAND_H

#include<string>
#include<WinSock2.h>

class Command
{
public:
    virtual bool Execute(SOCKET sock, char* buf) = 0;
    void SerializeInt(const int input, char* output);
    bool SendLength(SOCKET& sock, int length);
    bool Send(SOCKET& sock, const char* message);
    bool Recv(SOCKET& sock, char* outputString, int* size);
    int DeserializeInt(const char* input);
    bool RecvLength(SOCKET& sock, int* output);
};

Command* GetCommand(const std::string& command); // factory pattern

#endif // SERVER_COMMAND_H