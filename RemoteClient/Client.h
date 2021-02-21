#pragma once
#include<WinSock2.h>
#include<string>
#include"CustomPacket.h"
#include<filesystem>
#include"Receiver.h"
#include"PacketCommand.h";

namespace fs = std::filesystem;

#pragma comment(lib, "ws2_32.lib")

class Client
{
public:
    enum { UPLOAD = 1, DOWNLOAD = 2, END = 3, BUF_SIZE = 4096, PORT = 9000 };

    Client();
    void Initialize();
    void Upload();

    // 클라이언트에서 사용할 패킷 구조체 선언
    CustomPacket packet;

    const unsigned int dataSize = BUF_SIZE - packet.GetHeaderSize();

    // send 버퍼 선언
    char sendBuffer[BUF_SIZE];

private:
    // client 소켓 선언
    SOCKET clientSock;

    // 서버주소 구조체 변수 선언
    SOCKADDR_IN serverAddress;

    std::vector<std::unique_ptr<PacketCommand>> commands;

    void AddPacketCommand(PacketCommand* command);
};