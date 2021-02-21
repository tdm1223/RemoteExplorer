#pragma once
#include<WinSock2.h>
#include<string>
#include<filesystem>
#include"Receiver.h"
#include"PacketCommand.h";

namespace fs = std::filesystem;

#pragma comment(lib, "ws2_32.lib")

class Client
{
public:
    Client();
    void Initialize();
private:
    const int PORT = 9000;
    // client 소켓 선언
    SOCKET clientSock;

    // 서버주소 구조체 변수 선언
    SOCKADDR_IN serverAddress;

    std::vector<std::unique_ptr<PacketCommand>> commands;

    void AddPacketCommand(PacketCommand* command);
};