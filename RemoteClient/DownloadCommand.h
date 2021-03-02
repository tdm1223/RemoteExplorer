#ifndef DOWNLOAD_COMMAND_H
#define DOWNLOAD_COMMAND_H
#include<iostream>
#include "PacketCommand.h"

class DownloadCommand : public PacketCommand
{
    bool Execute(SOCKET sock);
};
#endif