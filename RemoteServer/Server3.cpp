#include<stdio.h>
#include<string.h>
#include<winsock2.h>
#include<queue>

#pragma comment(lib,"ws2_32")

#define PORT 9000
#define ServerSize 1024
#define MAX_MSG_LEN 256

SOCKET  socketArray[FD_SETSIZE];
HANDLE eventArray[FD_SETSIZE];
int numOfClient;

// 대기 소켓 설정
SOCKET SetServer(short pnum, int blog);

// Event 처리 Loop
void EventLoop(SOCKET sock);

HANDLE AddNetworkEvent(SOCKET sock, long net_event);

void AcceptProc(int index);
void ReadProc(int index);
void CloseProc(int index);

int main()
{
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata); // 윈속 초기화
    SOCKET sock = SetServer(PORT, ServerSize); // 대기 소켓 설정
    if (sock == -1)
    {
        perror("대기 소켓 오류");
    }
    else
    {
        EventLoop(sock);
    }
    WSACleanup();
    return 0;
}

SOCKET SetServer(short pnum, int blog)
{
    SOCKET sock;
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1)
    {
        return -1;
    }

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
    if (listen(sock, blog) == -1)
    {
        return -1;
    }
    return sock;
}

void EventLoop(SOCKET sock)
{
    AddNetworkEvent(sock, FD_ACCEPT | FD_CLOSE);
    while (true)
    {
        // 이벤트 기다리기
        // 가장 처음 발생한 index를 반환
        int index = WSAWaitForMultipleEvents(numOfClient, eventArray, false, INFINITE, false);
        WSANETWORKEVENTS net_events;

        // 이벤트 종류 알아내기
        WSAEnumNetworkEvents(socketArray[index], eventArray[index], &net_events);
        switch (net_events.lNetworkEvents)
        {
        case FD_ACCEPT:
            AcceptProc(index);
            break;
        case FD_READ:
            ReadProc(index);
            break;
        case FD_CLOSE:
            CloseProc(index);
            break;
        }
    }
    closesocket(sock);
}

HANDLE AddNetworkEvent(SOCKET sock, long net_event)
{
    HANDLE wsaEvent = WSACreateEvent();

    socketArray[numOfClient] = sock;
    eventArray[numOfClient] = wsaEvent;
    numOfClient++;

    WSAEventSelect(sock, wsaEvent, net_event);
    return wsaEvent;
}

void AcceptProc(int index)
{
    SOCKADDR_IN cliaddr = { 0 };
    int len = sizeof(cliaddr);
    SOCKET dosock = accept(socketArray[0], (SOCKADDR*)&cliaddr, &len);

    if (numOfClient == FD_SETSIZE)
    {
        closesocket(dosock);
        return;
    }

    AddNetworkEvent(dosock, FD_READ | FD_CLOSE);
    printf("[%s:%d] 입장\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
}

void ReadProc(int index)
{
    char msg[MAX_MSG_LEN];
    recv(socketArray[index], msg, MAX_MSG_LEN, 0);

    SOCKADDR_IN cliaddr = { 0 };
    int len = sizeof(cliaddr);
    getpeername(socketArray[index], (SOCKADDR*)&cliaddr, &len);

    if (strcmp(msg, "exit") != 0)
    {
        char smsg[MAX_MSG_LEN];
        sprintf_s(smsg, "[%s:%d]:%s", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), msg);

        // 채팅
        for (int i = 1; i < numOfClient; i++)
        {
            send(socketArray[i], smsg, MAX_MSG_LEN, 0);
        }
    }
}

void CloseProc(int index)
{
    SOCKADDR_IN cliaddr = { 0 };
    int len = sizeof(cliaddr);
    getpeername(socketArray[index], (SOCKADDR*)&cliaddr, &len);
    printf("[%s:%d] 나감\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

    closesocket(socketArray[index]);
    WSACloseEvent(eventArray[index]);

    numOfClient--;
    socketArray[index] = socketArray[numOfClient];
    eventArray[index] = eventArray[numOfClient];

    char msg[MAX_MSG_LEN];
    sprintf_s(msg, "[%s:%d] 나감", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
    // 나간것 알려줌
    for (int i = 1; i < numOfClient; i++)
    {
        send(socketArray[i], msg, MAX_MSG_LEN, 0);
    }
}
