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

SOCKET  socketArray[FD_SETSIZE];
HANDLE eventArray[FD_SETSIZE];
int numOfClient;

// 대기 소켓 설정
SOCKET SetServer(short pnum, int blog);

void EventLoop(SOCKET sock);

// 클라이언트 소켓 등록하는 함수
void AddEvent(SOCKET sock, long net_event);

unsigned WINAPI AcceptProc(void* param);

unsigned WINAPI ReadProc(void* param);
unsigned WINAPI CloseProc(void* param);

//파일 기본 정보
struct Files
{
    char name[MAX_MSG_LEN];
    unsigned int size;
};

int main()
{
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        exit(1);
    }
    SOCKET sock = SetServer(PORT, ServerSize); // 대기 소켓 설정
    if (sock == -1)
    {
        perror("대기 소켓 오류");
    }
    else
    {
        std::cout << "Create Main Thread..." << std::endl;
        EventLoop(sock);
    }
    WSACleanup();
    return 0;
}

SOCKET SetServer(short pnum, int size)
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1)
    {
        return -1;
    }

    // 서버 정보 설정
    SOCKADDR_IN servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // 소켓 주소와 네트워크 인터페이스 결합
    if (bind(sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
    {
        return -1;
    }

    //백 로그 큐 설정
    if (listen(sock, size) == -1)
    {
        return -1;
    }
    return sock;
}

void EventLoop(SOCKET sock)
{
    AddEvent(sock, FD_ACCEPT | FD_CLOSE);
    unsigned int acceptThreadId, readThreadId, closeThreadId;
    HANDLE acceptThread, readThread, closeThread;
    while (true)
    {
        // 이벤트 발생을 기다리면서 가장 처음 발생한 index를 반환
        int index = WSAWaitForMultipleEvents(numOfClient, eventArray, false, INFINITE, false);
        WSANETWORKEVENTS net_events;

        // 이벤트 종류 알아내기
        WSAEnumNetworkEvents(socketArray[index], eventArray[index], &net_events);
        switch (net_events.lNetworkEvents)
        {
        case FD_ACCEPT:
            acceptThread = (HANDLE)_beginthreadex(NULL, 0, AcceptProc, (void*)index, 0, (unsigned*)&acceptThreadId);
            WaitForSingleObject(acceptThread, INFINITE);
            CloseHandle(acceptThread);
            break;
        case FD_READ:
            std::cout << "요청 클라이언트 인덱스 : " << index << std::endl;
            readThread = (HANDLE)_beginthreadex(NULL, 0, ReadProc, (void*)index, 0, (unsigned*)&readThreadId);
            WaitForSingleObject(readThread, INFINITE);
            CloseHandle(readThread);
            break;
        case FD_CLOSE:
            closeThread = (HANDLE)_beginthreadex(NULL, 0, CloseProc, (void*)index, 0, (unsigned*)&closeThreadId);
            WaitForSingleObject(closeThread, INFINITE);
            CloseHandle(closeThread);
            break;
        }
    }
    closesocket(sock);
}

void AddEvent(SOCKET sock, long eventType)
{
    HANDLE wsaEvent = WSACreateEvent();
    socketArray[numOfClient] = sock;
    eventArray[numOfClient] = wsaEvent;
    numOfClient++;
    WSAEventSelect(sock, wsaEvent, eventType);
}

unsigned WINAPI AcceptProc(void* param)
{
    SOCKADDR_IN clientAddress = { 0 };
    int len = sizeof(clientAddress);
    SOCKET sock = accept(socketArray[0], (SOCKADDR*)&clientAddress, &len);

    if (numOfClient == FD_SETSIZE)
    {
        closesocket(sock);
        return 0;
    }
    AddEvent(sock, FD_READ | FD_CLOSE | FD_WRITE);
    std::cout << "[" << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "] 연결 성공" << std::endl;
    return 0;
}

unsigned WINAPI ReadProc(void* param)
{
    Files recvFile;
    int index = (int)param;

    // 파일 기본 정보를 수신
    int retval = recv(socketArray[index], (char*)&recvFile, sizeof(recvFile), 0);
    if (retval == SOCKET_ERROR)
    {
        return 0;
    }
    std::cout << "전송하는 파일 : " << recvFile.name << " 전송하는 파일 크기 : " << recvFile.size << "Byte" << std::endl;

    // 기존 파일 여부 확인
    FILE* fp = fopen(recvFile.name, "rb");
    if (fp == NULL)
    {
        std::cout << "파일명이 같은 파일이 존재하지 않습니다" << std::endl;
    }
    else
    {
        std::cout << "파일명이 같은 파일이 존재합니다" << std::endl;
    }

    // 데이터 받아서 파일 쓰는 로직
    fp = fopen(recvFile.name, "wb");
    int numtotal = 0;
    char buf[BUF_SIZE];
    int count = 1;

    while (1)
    {
        retval = recv(socketArray[index], buf, BUF_SIZE, 0);
        if (retval == -1)
        {
            fclose(fp);
            break;
        }
        else
        {
            fwrite(buf, 1, retval, fp);
            numtotal += retval;
        }
    }

    if (numtotal == recvFile.size)
    {
        std::cout << "파일 전송이 완료되었습니다" << std::endl;
    }
    else
    {
        std::cout << "파일 전송에 문제가 있습니다" << std::endl;
    }
    return 0;
}

unsigned WINAPI CloseProc(void* param)
{
    int index = (int)param;
    SOCKADDR_IN clientAddress = { 0 };
    int len = sizeof(clientAddress);
    getpeername(socketArray[index], (SOCKADDR*)&clientAddress, &len);
    std::cout << "[" << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "] 연결 종료" << std::endl;

    closesocket(socketArray[index]);
    WSACloseEvent(eventArray[index]);

    numOfClient--;
    socketArray[index] = socketArray[numOfClient];
    eventArray[index] = eventArray[numOfClient];

    return 0;
}