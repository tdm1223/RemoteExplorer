#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>

#define BUFSIZE 4096

#pragma warning(disable:4996)
#pragma comment(lib, "Ws2_32.lib")

void ErrorHandling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

// 사용자 정의 데이터 수신 함수
int receiveFile(SOCKET s, char* buf, int len, int flags)
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


int main()
{
    // 윈속 초기화
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    SOCKADDR_IN serverAddress, clientAddress;
    TIMEVAL timeout;
    fd_set reads, cpyReads;

    int adrSz;
    int strLen, fdNum, i;
    char buf[BUFSIZE + 1];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        return -1;
    }

    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(9000);

    if (bind(serverSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        ErrorHandling("bind() error");
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR)
    {
        ErrorHandling("listen() error");
    }

    FD_ZERO(&reads);
    FD_SET(serverSocket, &reads);

    while (1)
    {
        cpyReads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        if ((fdNum = select(reads.fd_count, &cpyReads, 0, 0, &timeout)) == SOCKET_ERROR)
        {
            break;
        }

        if (fdNum == 0)
        {
            continue;
        }

        for (i = 0; i < reads.fd_count; i++)
        {
            if (FD_ISSET(reads.fd_array[i], &cpyReads))
            {
                if (reads.fd_array[i] == serverSocket) // connection request!
                {
                    adrSz = sizeof(clientAddress);
                    clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddress, &adrSz);
                    FD_SET(clientSocket, &reads);
                    printf("클라 접속 : IP = %s, Port = %d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
                }
                else
                {
                    //char fileName[256];
                    strLen = recv(reads.fd_array[i], buf, BUFSIZE, 0);
                    FILE* fp = fopen(buf, "wb");

                    if (strLen == 0)
                    {
                        FD_CLR(reads.fd_array[i], &reads);
                        closesocket(cpyReads.fd_array[i]);
                        printf("소켓 연결 종료 : %d \n", cpyReads.fd_array[i]);
                    }
                    else
                    {
                        printf("-> 받을 파일 이름: %s\n", buf);
                        printf("-> file pointer : %d, fopen() is working now\n", *fp);
                    }
                    fclose(fp);
                }
                    //// 파일 이름 받기 (Todo : 
                    //strLen = recv(reads.fd_array[i], buf, BUFSIZE - 1, 0);
                    //printf("-> 종류 : %s\n", buf);
                    //if (buf == "Close")
                    //{
                    //    FD_CLR(reads.fd_array[i], &reads);
                    //    closesocket(cpyReads.fd_array[i]);
                    //    printf("소켓 연결 종료 : %d \n", cpyReads.fd_array[i]);
                    //    continue;
                    //}
                    //else
                    //{
                    //    // 파일 이름 받기
                    //    char fileName[256];
                    //    ZeroMemory(fileName, 256);
                    //    strLen = recv(reads.fd_array[i], fileName, 256, 0);
                        //// 파일 크기 받기
                        //int totalbytes;
                        //strLen = recv(reads.fd_array[i], (char*)&totalbytes, sizeof(totalbytes), 0);
                        //if (strLen == SOCKET_ERROR)
                        //{
                        //    closesocket(clientSocket);
                        //    continue;
                        //}
                        //printf("-> 받을 파일 크기: %d\n", totalbytes);
                        //// 파일 열기
                        //FILE* fp = fopen(fileName, "wb");
                        //if (fp == NULL)
                        //{
                        //    perror("파일 입출력 오류");
                        //    closesocket(clientSocket);
                        //    continue;
                        //}
                        //else
                        //{
                        //    printf("파일 오픈 성공\n");
                        //}
                        //// 파일 데이터 받기
                        //char fileBuf[BUFSIZE + 1];
                        //int numTotal = 0;
                        //while (1)
                        //{
                        //    strLen = recv(reads.fd_array[i], fileBuf, BUFSIZE, 0);
                        //    printf("strLen : %d\n", strLen);
                        //    if (strLen == SOCKET_ERROR)
                        //    {
                        //        break;
                        //    }
                        //    else if (strLen == 0)
                        //    {
                        //        break;
                        //    }
                        //    else
                        //    {
                        //        fwrite(fileBuf, 1, strLen, fp);
                        //        if (ferror(fp))
                        //        {
                        //            perror("파일 입출력 오류\n");
                        //            break;
                        //        }
                        //        numTotal += strLen;
                        //        printf("numTotal : %d\n", numTotal);
                        //    }
                        //}
                        //fclose(fp);
                        //// 전송 결과 출력
                        //if (numTotal == totalbytes)
                        //{
                        //    printf("-> 파일 전송 완료!\n");
                        //}
                        //else
                        //{
                        //    printf("-> 파일 전송 실패!\n");
                        //}
            }
        }
    }
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}