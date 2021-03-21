#ifndef SERVER_DOWNLOAD_COMMAND_H
#define SERVER_DOWNLOAD_COMMAND_H

#include "Command.h"
#include<iostream>

class DownloadCommand : public Command
{
    bool Execute(SOCKET& sock);
};

#endif // SERVER_DOWNLOAD_COMMAND_H