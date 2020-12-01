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
    enum { UPLOAD = 1, DOWNLOAD = 2, END = 3, BUF_SIZE = 4096, PORT = 9000 };

    Client();
    void Start();
    // client 소켓 선언
    SOCKET clientSock;

    // 서버주소 구조체 변수 선언
    SOCKADDR_IN serverAddress;

    // 헤더 구조체 선언
    CustomPacket packet;
};

