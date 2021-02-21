#ifndef CLIENT_H
#define CLIENT_H

#include"Receiver.h"
#include"PacketCommand.h"
#include<iostream>
#include<vector>

class Client
{
public:
    Client();
    std::vector<std::unique_ptr<PacketCommand>> commands;
private:
    Receiver receiver;
    void AddPacketCommand(PacketCommand* command);

};
#endif
