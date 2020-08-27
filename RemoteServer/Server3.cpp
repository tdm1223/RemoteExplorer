#include<stdio.h>
#include<string.h>
#include<winsock2.h>
#include<queue>
#include<thread>

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
        printf("Create Main Thread...\n");
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
    printf("[%s:%d] 입장\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
    return 0;
}

unsigned WINAPI ReadProc(void* param)
{
    FILE* fp = NULL;
    Files files;
    int index = (int)param;

    // 1. 파일 기본 정보를 받음
    int retval = recv(socketArray[index], (char*)&files, sizeof(files), 0);
    if (retval == SOCKET_ERROR)
    {
        return 0;
    }
    printf("전송하는 파일 : %s, 전송하는 파일 크기 : %d Byte\n", files.name, files.size);

    //// 기존 파일 여부 확인
    //fp = fopen(files.name, "rb");
    //if (fp == NULL) 
    //{
    //    printf("같은 파일 이름이 없으므로 전송을 진행합니다.\n");
    //}
    //else
    //{
    //    fclose(fp);
    //    return 0;
    //}

    //// 데이터 받아서 파일 쓰는 로직
    //unsigned int count = files.byte / BUF_SIZE;
    //char buf[BUF_SIZE]; //전송하는 데이터
    //fp = fopen(files.name, "wb");

    //while (count)
    //{
    //    retval = recv(socketArray[index], buf, BUF_SIZE, 0);
    //    printf("retval : %d\n", retval);
    //    if (retval == SOCKET_ERROR) 
    //    {
    //        return 0;
    //    }
    //    fwrite(buf, 1, BUF_SIZE, fp);
    //    count--;
    //}

    ////남은 파일 크기만큼 나머지 받기
    //count = files.byte - ((files.byte / BUF_SIZE) * BUF_SIZE);
    //retval = recv(socketArray[index], buf, BUF_SIZE, 0);
    //if (retval == SOCKET_ERROR) 
    //{
    //    return 0;
    //}
    //fwrite(buf, 1, count, fp);

    ////파일포인터 닫기
    //fclose(fp);
    printf("\n파일 전송이 완료되었습니다.\n");

    // 연결된 클라이언트 정보를 얻어온다.
    //SOCKADDR_IN clientAddress;
    //int clientLen = sizeof(clientAddress);
    //getpeername(socketArray[index], (SOCKADDR*)&clientAddress, &clientLen);

    //char smsg[MAX_MSG_LEN];
    //sprintf_s(smsg, "[%s:%d]:%s", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port), msg);

    //// 채팅
    //for (int i = 1; i < numOfClient; i++)
    //{
    //    send(socketArray[i], smsg, MAX_MSG_LEN, 0);
    //}


    return 0;
}

unsigned WINAPI CloseProc(void* param)
{
    int index = (int)param;
    SOCKADDR_IN cliaddr = { 0 };
    int len = sizeof(cliaddr);
    getpeername(socketArray[index], (SOCKADDR*)&cliaddr, &len);
    printf("[%s:%d] 연결 종료\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

    closesocket(socketArray[index]);
    WSACloseEvent(eventArray[index]);

    numOfClient--;
    socketArray[index] = socketArray[numOfClient];
    eventArray[index] = eventArray[numOfClient];

    return 0;
}
