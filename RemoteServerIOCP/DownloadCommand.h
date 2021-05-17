#ifndef SERVER_DOWNLOAD_COMMAND_H
#define SERVER_DOWNLOAD_COMMAND_H

#include "Command.h"

class DownloadCommand : public Command
{
    bool Execute(SOCKET sock, char* buf);
};

#endif // SERVER_DOWNLOAD_COMMAND_H