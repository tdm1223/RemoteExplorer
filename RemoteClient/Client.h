#ifndef CLIENT_H
#define CLIENT_H

#include<WinSock2.h>
#include"ClientSocket.h"
#include"ThreadPool.h"
#include<memory>

#pragma comment(lib, "ws2_32.lib")

class Client
{
public:
    Client();
    bool Initialize();
    void Start();
    void Close();
    char sendBuffer[Util::kBufferSize];
    std::unique_ptr<ThreadPool> threadPool;

private:
    ClientSocket* clientSocket;
    int numberOfThreads_;
};

#endif