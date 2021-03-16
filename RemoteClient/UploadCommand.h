#ifndef UPLOAD_COMMAND_H
#define UPLOAD_COMMAND_H

#include<iostream>
#include "PacketCommand.h"
class UploadCommand : public PacketCommand
{
    bool Execute(SOCKET sock, void* buffer);
};
#endif