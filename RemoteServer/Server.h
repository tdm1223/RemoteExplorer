﻿#ifndef SERVER_H
#define SERVER_H

#include<winsock2.h>
#include<thread>
#include<iostream>
#include<string>
#include<filesystem>
#include"ThreadPool.h"
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
    std::unique_ptr<ThreadPool> threadPool;
    SOCKET listenSock;
    int numOfClient;
    Invoker commandInvoker;

private:
    SOCKET SetServer(); // 대기 소켓 설정
    SOCKET clientSock;

    void AddEvent(SOCKET sock, long eventType); // 클라이언트 소켓 등록하는 함수
    void EventLoop(SOCKET sock);

    void GetClientAddress(SOCKADDR_IN& clientAddress, int index);
    void CloseProc(int index, int& numOfClient);

    int numberOfThreads_;
};

#endif