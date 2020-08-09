#include<stdio.h>
#include<WinSock2.h>

#define BUFSIZE 1024

#pragma warning(disable:4996)
#pragma comment(lib, "Ws2_32.lib")

int main()
{
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        return -1;
    }

    // 클라이언트 소켓 초기화
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

    // 서버에 연결
    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9000);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 보낼 파일 설정
    char sendFileName[256] = "test.txt";

    connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    printf("서버에 접속 성공 : IP = %s, Port = %d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));

    // 파일 열고 파일명 보내기
    FILE* fp = fopen(sendFileName, "rb");
    char fileName[256];
    ZeroMemory(fileName, 256);
    sprintf(fileName, sendFileName);
    send(sock, fileName, 256, 0);
    printf("보내는 파일의 이름 : %s\n", fileName);

    // 파일 크기 얻고 보내기
    fseek(fp, 0, SEEK_END);
    int totalbytes = ftell(fp);
    send(sock, (char*)&totalbytes, sizeof(totalbytes), 0);
    rewind(fp); // 파일 크기 얻고 파일 포인터를 제일 앞으로 이동시켜줌

    // 파일 보내기
    char buf[BUFSIZE];
    int numRead;
    int total = 0;
    while (1)
    {
        numRead = fread(buf, 1, BUFSIZE, fp);
        if (numRead > 0)
        {
            send(sock, buf, numRead, 0);
            total += numRead;
        }
        else if (numRead == 0 && total == totalbytes)
        {
            printf("총 %d 바이트 파일 전송을 완료했습니다.\n", total);
            break;
        }
    }
    fclose(fp);

    closesocket(sock);
    printf("접속 종료 : IP = %s, Port = %d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
    WSACleanup();
    return 0;
}