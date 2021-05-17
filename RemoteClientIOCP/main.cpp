#include <iostream>
#include "IOCPClient.h"

int main()
{
    IOCPClient* client = new IOCPClient();
    if (client->Connect("127.0.0.1", 9000))
    {
        while (TRUE)
        {
            int command;
            std::cout << "1 - UPLOAD" << std::endl;
            std::cout << "2 - DOWNLOAD" << std::endl;
            std::cout << "3 - LIST" << std::endl;
            std::cout << "4 - END" << std::endl;
            std::cin >> command;

            if (!client->Query(command))
            {
                break;
            }
        }
    }
    return 0;
}

//#include<stdio.h>
//#include<stdlib.h>
//#include<winsock2.h>
//#include<iostream>
//
//#define BUF_SIZE 1024
//
//int main()
//{
//    WSADATA wsaData;
//    SOCKET hSocket;
//    SOCKADDR_IN servAdr;
//    char message[BUF_SIZE];
//    int strLen, readLen;
//
//    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
//    {
//        std::cout << "start up error" << std::endl;
//    }
//
//    hSocket = socket(PF_INET, SOCK_STREAM, 0);
//    if (hSocket == INVALID_SOCKET)
//    {
//        std::cout << "hSocket error" << std::endl;
//    }
//
//    memset(&servAdr, 0, sizeof(servAdr));
//    servAdr.sin_family = AF_INET;
//    servAdr.sin_addr.s_addr = inet_addr("127.0.0.1");
//    servAdr.sin_port = htons(9000);
//
//    if (connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
//    {
//        std::cout << "connect error" << std::endl;
//    }
//    else
//    {
//        std::cout << "Connected..........." << std::endl;
//    }
//
//    while (1)
//    {
//        std::cout << "Input message(Q to quit): ";
//        std::cin >> message;
//        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
//        {
//            break;
//        }
//
//        strLen = strlen(message);
//        send(hSocket, message, strLen, 0);
//
//        readLen = 0;
//        while (1)
//        {
//            readLen += recv(hSocket, &message[readLen], BUF_SIZE - 1, 0);
//            if (readLen >= strLen)
//                break;
//        }
//        message[strLen] = 0;
//        printf("Message from server: %s", message);
//    }
//
//    closesocket(hSocket);
//    WSACleanup();
//    return 0;
//}