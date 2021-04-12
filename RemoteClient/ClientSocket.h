#ifndef CLIENT_SOCKET_H
#define CLIENT_SOCKET_H

#include<WinSock2.h>
#include<vector>
#include<memory>
#include"PacketCommand.h"

class Client;

class ClientSocket
{
public:
    virtual ~ClientSocket();
    static ClientSocket* GetInstance();
    static void releaseInstance();
    SOCKET& GetSocket();
    void CloseSocket();
    bool Connect(int port);
    SOCKET sock;
    char sendBuffer[Util::kBufferSize];
    std::vector<std::unique_ptr<PacketCommand>> packetCommands;

private:
    ClientSocket();
    void AddPacketCommand(PacketCommand* packetCommand);
    static ClientSocket* clientSocket;
    WSADATA wsaData;
    SOCKADDR_IN serverAddress;
    bool cleanSocket;

};

#endif