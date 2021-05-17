#ifndef DOWNLOAD_COMMAND_H
#define DOWNLOAD_COMMAND_H

#include "PacketCommand.h"

class DownloadCommand : public PacketCommand
{
    bool Execute(SOCKET sock, void* buffer);
};
#endif