#pragma once
#include<winsock2.h>
#include<thread>
#include<iostream>
#include<string>
#include<filesystem>
#include"Packet.h"
#include"Command.h"
#include"Invoker.h"

namespace fs = std::filesystem;

#pragma comment(lib,"ws2_32")
#pragma warning(disable: 4996)

class Server
{
public:
    Server();
    ~Server();
    SOCKET socketArray[FD_SETSIZE];
    HANDLE eventArray[FD_SETSIZE];
    int numOfClient;

    void GetClientAddress(SOCKADDR_IN& clientAddress, int index);
    void CloseProc(int index);
    SOCKET listenSock;
    Invoker commandInvoker;
private:
    enum { UPLOAD = 1, DOWNLOAD = 2, END = 3, TEST = 4, PORT = 9000, BUF_SIZE = 4096 };

    SOCKET SetServer(); // 대기 소켓 설정
    SOCKET clientSock;

    void AddEvent(SOCKET sock, long eventType); // 클라이언트 소켓 등록하는 함수
    void EventLoop(SOCKET sock);
};

