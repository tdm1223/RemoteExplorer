#ifndef SERVER_UPLOAD_COMMAND_H
#define SERVER_UPLOAD_COMMAND_H

#include"Command.h"
#include<iostream>

class UploadCommand : public Command
{
public:
    bool Execute(SOCKET& sock);
};

#endif // _DEBUG