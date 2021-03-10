#ifndef MOVECOMMAND_H
#define MOVECOMMAND_H
#include<iostream>
#include "PacketCommand.h"

class MoveCommand : public PacketCommand
{
    bool Execute(SOCKET s, void* buffer);
};
#endif