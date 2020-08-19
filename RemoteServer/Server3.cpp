#include<stdio.h>
#include<string.h>
#include<winsock2.h>

#pragma comment(lib,"ws2_32")

#define PORT_NUM      10200
#define BLOG_SIZE       5
#define MAX_MSG_LEN 256

SOCKET SetTCPServer(short pnum, int blog);//대기 소켓 설정
void EventLoop(SOCKET sock);//Event 처리 Loop

int main()
{
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2, 2), &wsadata);//윈속 초기화           
    SOCKET sock = SetTCPServer(PORT_NUM, BLOG_SIZE);//대기 소켓 설정
    if (sock == -1) { perror("대기 소켓 오류"); }
    else { EventLoop(sock); }
    WSACleanup();//윈속 해제화
    return 0;
}

SOCKET SetTCPServer(short pnum, int blog)
{
    SOCKET sock;
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//소켓 생성
    if (sock == -1) { return -1; }

    SOCKADDR_IN servaddr = { 0 };//소켓 주소
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT_NUM);

    int re = 0;
    re = bind(sock, (struct sockaddr*)&servaddr, sizeof(servaddr));//소켓 주소와 네트워크 인터페이스 결합
    if (re == -1) { return -1; }

    re = listen(sock, blog);//백 로그 큐 설정
    if (re == -1) { return -1; }
    return sock;
}

SOCKET  sock_base[FD_SETSIZE];
HANDLE hev_base[FD_SETSIZE];
int cnt;

HANDLE AddNetworkEvent(SOCKET sock, long net_event)
{
    HANDLE hev = WSACreateEvent();

    sock_base[cnt] = sock;
    hev_base[cnt] = hev;
    cnt++;

    WSAEventSelect(sock, hev, net_event);
    return hev;
}

void AcceptProc(int index);
void ReadProc(int index);
void CloseProc(int index);

void EventLoop(SOCKET sock)
{
    AddNetworkEvent(sock, FD_ACCEPT);
    while (true)
    {
        int index = WSAWaitForMultipleEvents(cnt, hev_base, false, INFINITE, false);
        WSANETWORKEVENTS net_events;
        WSAEnumNetworkEvents(sock_base[index], hev_base[index], &net_events);
        switch (net_events.lNetworkEvents)
        {
        case FD_ACCEPT: AcceptProc(index); break;
        case FD_READ: ReadProc(index); break;
        case FD_CLOSE: CloseProc(index); break;
        }
    }
    closesocket(sock);//소켓 닫기
}

void AcceptProc(int index)
{
    SOCKADDR_IN cliaddr = { 0 };
    int len = sizeof(cliaddr);
    SOCKET dosock = accept(sock_base[0], (SOCKADDR*)&cliaddr, &len);

    if (cnt == FD_SETSIZE)
    {
        printf("채팅 방에 꽉 차서 %s:%d 입장하지 못하였음!\n",
            inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
        closesocket(dosock);
        return;
    }

    AddNetworkEvent(dosock, FD_READ | FD_CLOSE);
    printf("%s:%d 입장\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
}

void ReadProc(int index)
{
    char msg[MAX_MSG_LEN];
    recv(sock_base[index], msg, MAX_MSG_LEN, 0);

    SOCKADDR_IN cliaddr = { 0 };
    int len = sizeof(cliaddr);
    getpeername(sock_base[index], (SOCKADDR*)&cliaddr, &len);

    char smsg[MAX_MSG_LEN];
    sprintf(smsg, "[%s:%d]:%s", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), msg);

    for (int i = 1; i < cnt; i++)
    {
        send(sock_base[i], smsg, MAX_MSG_LEN, 0);
    }
}

void CloseProc(int index)
{
    SOCKADDR_IN cliaddr = { 0 };
    int len = sizeof(cliaddr);
    getpeername(sock_base[index], (SOCKADDR*)&cliaddr, &len);
    printf("[%s:%d]  님 나감~\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

    closesocket(sock_base[index]);
    WSACloseEvent(hev_base[index]);

    cnt--;
    sock_base[index] = sock_base[cnt];
    hev_base[index] = hev_base[cnt];

    char msg[MAX_MSG_LEN];
    sprintf(msg, "[%s:%d]님 나감~\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
    for (int i = 1; i < cnt; i++)
    {
        send(sock_base[i], msg, MAX_MSG_LEN, 0);
    }
}
