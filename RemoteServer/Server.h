#pragma once
#include<stdio.h>
#include<winsock2.h>
#include<queue>
#include<thread>
#include<iostream>
#include<string>

#pragma comment(lib,"ws2_32")
#pragma warning(disable: 4996)

#define PORT 9000
#define ServerSize 1024
#define MAX_MSG_LEN 256
#define BUF_SIZE 8096

//���� �⺻ ����
struct Files
{
    char name[MAX_MSG_LEN];
    unsigned int size;
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
    void ReadProc(int num);
    void CloseProc(int num);
    void GetClientAddress(SOCKADDR_IN& clientAddress, int index);

private:
    // ��� ���� ����
    SOCKET SetServer(int size);

    // Ŭ���̾�Ʈ ���� ����ϴ� �Լ�
    void AddEvent(SOCKET sock, long eventType);

    void EventLoop(SOCKET sock);
};

