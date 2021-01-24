#pragma once
#include<WinSock2.h>
#include<string>
#include<iostream>
#include"Packet.h"
#include<filesystem>
#include"Parser.h"
#include <thread>

namespace fs = std::filesystem;

#pragma comment(lib, "ws2_32.lib")

class Client
{
public:
    enum { UPLOAD = 1, DOWNLOAD = 2, END = 3, TEST = 4, BUF_SIZE = 4096, PORT = 9000 };

    Client();
    void Start();
    // client 소켓 선언
    SOCKET clientSock;

    // 서버주소 구조체 변수 선언
    SOCKADDR_IN serverAddress;

    // 클라이언트에서 사용할 패킷 구조체 선언
    CustomPacket packet;

    const int dataSize = BUF_SIZE - packet.GetHeaderSize();
};

