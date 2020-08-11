#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 1024
#pragma comment(lib,"ws2_32")

void ErrorHandling(char* message);

int main()
{
    WSADATA wsaData;
    SOCKET clientSocket;
    char message[BUF_SIZE];
    int strLen;
    SOCKADDR_IN serverAddress;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        ErrorHandling("WSAStartup() error!");
    }

    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        ErrorHandling("socket() error");
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(9000);

    if (connect(clientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        ErrorHandling("connect() error!");
    }
    else
    {
        printf("서버에 접속 성공 : IP = %s, Port = %d\n", inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port));
    }

    while (1)
    {
        fputs("Input message(Q to quit): ", stdout);
        fgets(message, BUF_SIZE, stdin);
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
        {
            break;
        }
        send(clientSocket, message, strlen(message), 0);
        strLen = recv(clientSocket, message, BUF_SIZE - 1, 0);
        message[strLen] = 0;
        printf("Message from server: %s", message);
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}

void ErrorHandling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}