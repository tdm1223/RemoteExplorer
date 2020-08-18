#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000
#define BUFSIZE    8192

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET, char*, int, int);

//파일 기본 정보
struct Files 
{
    char name[255];
    unsigned int byte;
};

int main()
{
    //send, recv 함수 출력값 저장용
    int retval;

    //(파일 크기 / 버퍼 사이즈) 계산한 값을 while문으로 돌리기 위한 변수
    unsigned int count;

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // socket()
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET)
    {
        exit(1);
    }

    // bind()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR)
    {
        exit(1);
    }

    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR)
    {
        exit(1);
    }

    // 데이터 통신에 사용할 변수
    SOCKET client_sock;  //클라이언트 저장 소켓
    SOCKADDR_IN clientaddr; //클라이언트 주소 저장
    int addrlen;   //주소 길이
    char buf[BUFSIZE]; //전송하는 데이터

    while (1)
    {
        // accept()
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) 
        {
            break;
        }

        // 접속한 클라이언트 정보 출력
        printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
            inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

        //클라이언트로 부터 파일 기본 정보 받기
        FILE* fp = NULL;
        Files files;

        retval = recvn(client_sock, (char*)&files, sizeof(files), 0);
        if (retval == SOCKET_ERROR) 
        {
            exit(1);
        }

        //기존 파일 여부 확인
        fp = fopen(files.name, "rb");
        if (fp == NULL) { printf("같은 파일 이름이 없으므로 전송을 진행합니다.\n"); }
        else
        {
            system("cls");
            printf("이미 같은 이름의 파일이 존재 합니다.\n전송을 종료합니다.\n");
            fclose(fp);
            break;
        }

        printf("파일을 전송받습니다.\n");
        printf("전송하는 파일 : %s, 전송하는 파일 크기 : %d Byte\n", files.name, files.byte);
        printf("\n클라이언트로 부터 파일을 전송 받는 중 입니다.\n");

        fp = fopen(files.name, "wb");

        count = files.byte / BUFSIZE;

        while (count)
        {
            //받기
            retval = recvn(client_sock, buf, BUFSIZE, 0);
            printf("retval : %d\n", retval);
            if (retval == SOCKET_ERROR) {
                exit(1);
            }

            //파일 작성 작업
            fwrite(buf, 1, BUFSIZE, fp);

            count--;
        }

        //남은 파일 크기만큼 나머지 받기
        count = files.byte - ((files.byte / BUFSIZE) * BUFSIZE);

        retval = recvn(client_sock, buf, BUFSIZE, 0);
        if (retval == SOCKET_ERROR) {
            exit(1);
        }

        fwrite(buf, 1, count, fp);

        //파일포인터 닫기
        fclose(fp);
        printf("\n파일 전송이 완료되었습니다.\n");
    }

    // closesocket()
    closesocket(listen_sock);

    // 윈속 종료
    WSACleanup();

    return 0;
}

int recvn(SOCKET s, char* buf, int len, int flags)
{
    int received;
    char* ptr = buf;
    int left = len;

    while (left > 0)
    {
        received = recv(s, ptr, left, flags);
        if (received == SOCKET_ERROR)
        {
            return SOCKET_ERROR;
        }
        else if (received == 0)
        {
            break;
        }
        left -= received;
        ptr += received;
    }

    return (len - left);
}