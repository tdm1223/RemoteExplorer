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
    // client ���� ����
    SOCKET clientSock;

    // �����ּ� ����ü ���� ����
    SOCKADDR_IN serverAddress;

    // ��� ����ü ����
    CustomPacket packet;
};

