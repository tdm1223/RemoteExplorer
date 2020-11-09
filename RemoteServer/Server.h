#pragma once
#include<stdio.h>
#include<winsock2.h>
#include<queue>
#include<thread>
#include<iostream>
#include<string>
#include<filesystem>
#include "Parser.h"
#include "Packet.h"

namespace fs = std::filesystem;

#pragma comment(lib,"ws2_32")
#pragma warning(disable: 4996)

#define PORT 9000
#define MAX_MSG_LEN 256
#define BUF_SIZE 4096
#define MESSAGE_SIZE 20

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
    void DownloadProc(int index, Packet result, char* recvBuffer, int byteLen);
private:
    enum { UPLOAD = 1 , DOWNLOAD = 2, END = 3};

    // 대기 소켓 설정
    SOCKET SetServer();
    SOCKET clientSock;
    // 클라이언트 소켓 등록하는 함수
    void AddEvent(SOCKET sock, long eventType);

    void EventLoop(SOCKET sock);
};

