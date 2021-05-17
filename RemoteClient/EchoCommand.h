#ifndef UPLOAD_COMMAND_H
#define UPLOAD_COMMAND_H

#include "PacketCommand.h"

class EchoCommand : public PacketCommand
{
    bool Execute(SOCKET sock, void* buffer);
};
#endif