#ifndef SERVER_H
#define SERVER_H

#include<winsock2.h>
#include<thread>
#include<iostream>
#include<string>
#include<filesystem>
#include"ThreadPool.h"
#include"Invoker.h"
#include<thread>

#include"NetworkContext.h"
#include"NetworkSession.h"

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

    bool Start();
private:
    SOCKET listenSocket_;

    void AddEvent(SOCKET sock, long eventType); // 클라이언트 소켓 등록하는 함수
    void EventLoop(SOCKET sock);

    void GetClientAddress(SOCKADDR_IN& clientAddress, int index);
    void CloseProc(int index, int& numOfClient);

    int numberOfThreads_;

    static void WorkerThreadEntry_(void* server);
    std::vector<std::thread*> workerThreads_;
    HANDLE completionPortHandle_;
    void WorkerThread_();
    HANDLE shutdownEvent_ = nullptr;
    bool isServerRunning_ = false;

    void AcceptThread_();
    void AcceptConnection_();
    static void AcceptThreadEntry_(void* server);
    WSAEVENT acceptEvent_ = nullptr;
    bool InitializeIOCP_();
    std::thread* acceptThread_ = nullptr;
    int32_t Send_(NetworkSession* session, char* message, int32_t length);
    bool SendLength_(NetworkSession* session, int32_t length);

};

#endif