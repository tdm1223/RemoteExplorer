#pragma once
#include<WinSock2.h>
#include"ClientSocket.h"

#pragma comment(lib, "ws2_32.lib")

class Client
{
public:
    Client();
    void Initialize();
private:
    const int kPort = 9000;
    std::vector<std::unique_ptr<PacketCommand>> commands;
    ClientSocket* clientSock;
};