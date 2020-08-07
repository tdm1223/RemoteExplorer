#include<stdlib.h>
#include<stdio.h>
#include<WinSock2.h>

#define BUFSIZE 4096

#pragma warning(disable:4996)
#pragma comment(lib, "Ws2_32.lib")

// 데이터 수신 함수
int ReceiveData(SOCKET s, char* buf, int len, int flags)
{
    // 통신에 필요한 변수 선언
    int received;
    char *ptr = buf;
    int left = len;

    // 남아있는 데이터가 있는경우 반복 수행
    while (left > 0)
    {
        received = recv(s, ptr, left, flags);
        // 더이상 데이터를 받지 못하는 경우 종료
        if (received == 0)
        {
            break;
        }
        // 받아온 데이터가 존재한다면
        // 받은만큼 left 감소
        // 반은만큼 ptr 증가
        left -= received;
        ptr += received;
    }

    // 더 받을 수 있는 데이터의 길이를 반환
    return (len - left);
}

int main()
{
    // 데이터 크기를 담을 변수
    int retval;

    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // 소켓 생성
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

    SOCKADDR_IN servaddr;
    ZeroMemory(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9000);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(listenSocket, (SOCKADDR*)&servaddr, sizeof(servaddr));

    listen(listenSocket, SOMAXCONN);

    // 데이터 통신에 사용할 변수
    SOCKET clientSocket;
    SOCKADDR_IN clientAddr;
    int addrLen;
    char buf[BUFSIZE];

    // 반복적으로 클라이언트로부터 접속 요청을 받음
    while (1)
    {
        addrLen = sizeof(clientAddr);
        clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);

        printf("클라 접속 : IP = %s, Port = %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

        // 파일 이름 받기
        char fileName[256];
        ZeroMemory(fileName, 256);
        ReceiveData(clientSocket, fileName, 256, 0);
        printf("받을 파일 이름:%s\n", fileName);

        // 파일 크기 받기
        int totalBytes;
        ReceiveData(clientSocket, (char*)&totalBytes, sizeof(totalBytes), 0);
        printf("받을 파일 크기 : %d 바이트\n", totalBytes);

        // 파일 열고 데이터 받기
        FILE *fp = fopen(fileName, "wb");
        int total = 0;
        while (1)
        {
            retval = ReceiveData(clientSocket, buf, BUFSIZE, 0);
            // 더 받을 데이터가 없음
            if (retval == 0)
            {
                break;
            }
            else
            {
                fwrite(buf, 1, retval, fp);
                total += retval;
            }
        }

        fclose(fp);

        if (total == totalBytes)
        {
            printf("파일을 성공적으로 받았습니다.\n");
        }
        else
        {
            printf("파일을 제대로 받지 못했습니다.\n");
        }

        closesocket(clientSocket);
        printf("클라이언트 종료 : IP : %s, Port = %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
    }

    closesocket(listenSocket);
    WSACleanup();
    return 0;
}

