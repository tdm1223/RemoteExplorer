﻿#pragma once
#include<stdio.h>
#include<winsock2.h>
#include<queue>
#include<thread>
#include<iostream>
#include<string>
#include<condition_variable>  // std::condition_variable
#include<mutex>
#include<filesystem>

namespace fs = std::filesystem;

#pragma comment(lib,"ws2_32")
#pragma warning(disable: 4996)

#define PORT 9000
#define ServerSize 1024
#define MAX_MSG_LEN 256
#define BUF_SIZE 8096
#define MESSAGE_SIZE 20

//파일 기본 정보
struct Files
{
    char name[MAX_MSG_LEN];
    unsigned int size;
    int index;
};

class Server
{
public:
    Server();
    ~Server();

    SOCKET socketArray[FD_SETSIZE];
    HANDLE eventArray[FD_SETSIZE];
    int numOfClient;

    void AcceptProc();
    void ReadProc(int num, std::queue<Files>* queue, std::mutex* m, std::condition_variable* cv);
    void CloseProc(int num);
    void RecvProc(std::queue<Files>* queue, std::mutex* m, std::condition_variable* cv);
    void GetClientAddress(SOCKADDR_IN& clientAddress, int index);
    std::queue<Files> queue;

    std::mutex m;
    std::condition_variable cv;

private:
    // 대기 소켓 설정
    SOCKET SetServer(int size);

    // 클라이언트 소켓 등록하는 함수
    void AddEvent(SOCKET sock, long eventType);

    void EventLoop(SOCKET sock);
};

