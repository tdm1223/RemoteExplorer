#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>

#define PORT 9000
#define MAX_MSG_LEN 256
#define SERVER_IP "127.0.0.1"
#define BUF_SIZE 8096

unsigned ReceiveProc(void* param);
unsigned SendProc(void* param);

//파일 기본 정보
struct Files
{
    char name[MAX_MSG_LEN];
    unsigned int byte;
};

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
    //char msg[MAX_MSG_LEN] = "";
    FILE* fp;
    Files files;

    while (true)
    {
        gets_s(files.name, MAX_MSG_LEN);
        
        // 송신
        if (strcmp(files.name, "exit") == 0)
        {
            closesocket(sock); // 소켓 닫기
            return 0;
        }

        fp = fopen(files.name, "rb");
        if (fp == NULL)
        {
            printf("FILE Pointer ERROR\n");
            continue;
        };

        // 파일 끝으로 위치 옮김
        fseek(fp, 0L, SEEK_END);

        // 파일 바이트값 출력
        files.byte = ftell(fp);

        // 다시 파일 처음으로 위치 옮김
        fseek(fp, 0L, SEEK_SET);

        // 1. 파일 기본 정보 전송
        printf("전송하는 파일 : %s, 전송하는 파일 크기 : %d Byte\n", files.name, files.byte);
        send(sock, (char*)&files, sizeof(files), 0);

        // 데이터 통신에 사용할 변수
        char buf[BUF_SIZE];
        int retval;
        unsigned int count = files.byte / BUF_SIZE;

        while (count)
        {
            //파일 읽어서 버퍼에 저장
            fread(buf, 1, BUF_SIZE, fp);

            //전송
            retval = send(sock, buf, BUF_SIZE, 0);
            if (retval == SOCKET_ERROR)
            {
                return 0;
            }

            Sleep(100);
            count--;
        }

        //남은 파일 크기만큼 나머지 전송
        count = files.byte - ((files.byte / BUF_SIZE) * BUF_SIZE);
        fread(buf, 1, count, fp);

        retval = send(sock, buf, BUF_SIZE, 0);
        if (retval == SOCKET_ERROR)
        {
            return 0;
        }

        //파일포인터 닫기
        fclose(fp);
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
