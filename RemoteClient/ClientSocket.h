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
    void Connect(int port);

private:
    ClientSocket();
    void AddPacketCommand(PacketCommand* packetCommand);
    static ClientSocket* clientSocket;
    SOCKET sock;
    WSADATA wsaData;
    SOCKADDR_IN serverAddress;
    bool cleanSocket;

    DWORD flags;
    DWORD recvBytes;

    std::vector<std::unique_ptr<PacketCommand>> packetCommands;
};

#endif