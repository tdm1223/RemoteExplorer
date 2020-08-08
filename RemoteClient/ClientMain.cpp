#include<winsock2.h>
#include<stdlib.h>
#include<stdio.h>

#define BUFSIZE 4096

#pragma warning(disable:4996)
#pragma warning(disable:6319)

#pragma comment(lib, "Ws2_32.lib")

int main()
{
    // 윈속 초기화
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        return -1;
    }

    // 소켓 생성
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        return -1;
    }

    // 소켓 연결
    SOCKADDR_IN serverAddress;
    ZeroMemory(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(9000);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    int retval = connect(clientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress));
    if (retval == SOCKET_ERROR)
    {
        return -1;
    }

    while (1)
    {
        int k;
        scanf("%d", &k);
        if (k == 1)
        {
            // 파일 열기
            char sendFile[256] = "test.txt";
            FILE* fp = fopen(sendFile, "rb");
            if (fp == NULL) {
                perror("파일 입출력 오류");
                return -1;
            }

            // 파일 이름 보내기
            char filename[256];
            ZeroMemory(filename, 256);
            sprintf(filename, sendFile);
            retval = send(clientSocket, filename, 256, 0);
            if (retval == SOCKET_ERROR)
            {
                return -1;
            }

            // 파일 크기 얻기
            fseek(fp, 0, SEEK_END);
            int totalbytes = ftell(fp);

            // 파일 크기 보내기
            retval = send(clientSocket, (char*)&totalbytes, sizeof(totalbytes), 0);
            if (retval == SOCKET_ERROR)
            {
                return -1;
            }

            // 파일 데이터 전송에 사용할 변수
            char buf[BUFSIZE];
            int numread;
            int numtotal = 0;

            // 파일 데이터 보내기
            rewind(fp); // 파일 포인터를 제일 앞으로 이동
            while (1) {
                numread = fread(buf, 1, BUFSIZE, fp);
                if (numread > 0) {
                    retval = send(clientSocket, buf, numread, 0);
                    if (retval == SOCKET_ERROR) {
                        return -1;
                        break;
                    }
                    numtotal += numread;
                }
                else if (numread == 0 && numtotal == totalbytes) {
                    printf("파일 전송 완료!: %d 바이트\n", numtotal);
                    break;
                }
                else {
                    perror("파일 입출력 오류");
                    break;
                }
            }
            fclose(fp);
        }
        else
        {
            // 연결 종료
            closesocket(clientSocket);
            break;
        }
    }

    // 윈속 종료
    WSACleanup();
    return 0;
}