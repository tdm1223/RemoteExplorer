#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>

#define PORT 9000
#define MAX_MSG_LEN 256
#define SERVER_IP "127.0.0.1"

unsigned ReceiveProc(void* param);
unsigned SendProc(void* param);

int main()
{
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata);//윈속 초기화

    // 서버와 통신용 소켓 생성
    SOCKET sock;
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1)
    {
        return -1;
    }

    SOCKADDR_IN serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddress.sin_port = htons(PORT);

    // 연결 요청
    if (connect(sock, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
    {
        return -1;
    }

    unsigned sendThreadId, receiceThreadId;
    HANDLE sendThread = (HANDLE)_beginthreadex(NULL, 0, SendProc, (void*)sock, 0, &sendThreadId);
    HANDLE receiveThread = (HANDLE)_beginthreadex(NULL, 0, ReceiveProc, (void*)sock, 0, &receiceThreadId);

    // 보내는 스레드가 종료할때까지 대기
    WaitForSingleObject(sendThread, INFINITE);

    CloseHandle(sendThread);
    CloseHandle(receiveThread);
    WSACleanup(); // 윈속 해제화
    return 0;
}

unsigned WINAPI SendProc(void* param)
{
    SOCKET sock = (SOCKET)param;
    char msg[MAX_MSG_LEN] = "";
    while (true)
    {
        gets_s(msg, MAX_MSG_LEN);
        // 송신
        if (strcmp(msg, "exit") == 0)
        {
            closesocket(sock); // 소켓 닫기
            return 0;
        }
        send(sock, msg, sizeof(msg), 0);
    }
}

unsigned WINAPI ReceiveProc(void* param)
{
    SOCKET sock = (SOCKET)param;
    char msg[MAX_MSG_LEN];
    while (recv(sock, msg, MAX_MSG_LEN, 0) > 0)
    {
        printf("%s\n", msg);
    }
    closesocket(sock);
    return 0;
}
