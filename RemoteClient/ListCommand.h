#ifndef LISTCOMMAND_H
#define LISTCOMMAND_H
#include<iostream>
#include "PacketCommand.h"

class ListCommand : public PacketCommand
{
    bool Execute(SOCKET sock, void* buffer);
};
#endif