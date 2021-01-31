#ifndef UPLOAD_COMMAND
#define UPLOAD_COMMAND

#include"Command.h"
#include<iostream>

class Upload : public Command
{
public:
    void Execute(SOCKET& sock, std::string& name);
    void UploadProc(SOCKET sock, std::string name);
    int recvLength = 0;
    char recvBuffer[4096];
};

#endif UPLOAD_COMMAND