#ifndef ECHO_COMMAND_H
#define ECHO_COMMAND_H

#include "PacketCommand.h"

class EchoCommand : public PacketCommand
{
    bool Execute(SOCKET sock, void* buffer);
};
#endif