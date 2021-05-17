#ifndef IOCP_CLIENT_H
#define IOCP_CLIENT_H

#include<string>
#include<WinSock2.h>
#include <WS2tcpip.h>
#include"PacketCommand.h"

#pragma comment(lib,"ws2_32.lib")

class IOCPClient
{
public:
    IOCPClient();
    ~IOCPClient();
    bool Connect(const std::string& ip, const int port);
    bool Disconnect();
    bool Query(const char* message);
    bool Query(int command);

    HANDLE hCompletionPort;

    std::vector<std::unique_ptr<PacketCommand>> packetCommands;
    void AddPacketCommand(PacketCommand* packetCommand);

private:
    size_t connectSocket_;
    bool isConnected_;
    bool Send_(const char* message);
    bool SendLength_(int32_t length);
    bool Recv_(char* outputString, int32_t* size);
    bool RecvLength_(int32_t* outputInt);
    void ParseResponse_(char* message, int32_t length) const;
};

#endif