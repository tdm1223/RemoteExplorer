#include "Server.h"

Server::Server()
{
    numOfClient = 0;
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        exit(1);
    }
    SOCKET sock = SetServer(ServerSize); // 대기 소켓 설정
    if (sock == -1)
    {
        perror("대기 소켓 오류");
    }
    else
    {
        std::cout << "Create Main Thread..." << std::endl;
        EventLoop(sock);
    }
}

Server::~Server()
{
    WSACleanup();
}

SOCKET Server::SetServer(int size)
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

void Server::EventLoop(SOCKET sock)
{
    AddEvent(sock, FD_ACCEPT | FD_CLOSE);
    while (true)
    {
        // 이벤트 발생을 기다리면서 가장 처음 발생한 index를 반환
        int index = WSAWaitForMultipleEvents(numOfClient, eventArray, false, INFINITE, false);
        WSANETWORKEVENTS net_events;
        // 이벤트 종류 알아내기
        WSAEnumNetworkEvents(socketArray[index], eventArray[index], &net_events);
        if (net_events.lNetworkEvents == FD_ACCEPT)
        {
            SOCKADDR_IN clientAddress = { 0 };
            int len = sizeof(clientAddress);
            SOCKET sock = accept(socketArray[0], (SOCKADDR*)&clientAddress, &len);

            if (numOfClient == FD_SETSIZE)
            {
                closesocket(sock);
            }
            AddEvent(sock, FD_READ | FD_CLOSE);
            std::cout << "[" << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "] 연결 성공" << std::endl;
        }
        else if (net_events.lNetworkEvents == FD_READ)
        {
            std::cout << "요청 들어옴" << std::endl;
            Files recvFile;

            SOCKADDR_IN clientAddress = { 0 };
            GetClientAddress(clientAddress, index);

            // 파일 이름 크기 수신
            int fileLength = 0;
            recv(socketArray[index], (char*)&fileLength, sizeof(int), 0);
            std::cout << "file length : " << fileLength << std::endl;

            // 파일 기본 정보를 수신
            //int retval = recv(socketArray[index], (char*)&recvFile, sizeof(recvFile), 0);
            //std::cout << "retval : " << retval << std::endl;
            //if (retval == SOCKET_ERROR)
            //{
            //    return;
            //}
            //std::cout << "[" << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "] 전송하는 파일 : " << recvFile.name << " 전송하는 파일 크기 : " << recvFile.size << "Byte" << std::endl;

            //// 파일 받는 로직
            //FILE* fp = fopen(recvFile.name, "rb");
            //int numtotal = 0;
            //char buf[BUF_SIZE];
            //if (fp == NULL)
            //{
            //    // 데이터 받아서 파일 쓰는 로직
            //    fp = fopen(recvFile.name, "wb");
            //    while (1)
            //    {
            //        retval = recv(socketArray[index], buf, BUF_SIZE, 0);
            //        if (retval == -1)
            //        {
            //            break;
            //        }
            //        else
            //        {
            //            fwrite(buf, 1, retval, fp);
            //            numtotal += retval;
            //        }
            //    }
            //    if (numtotal == recvFile.size)
            //    {
            //        std::cout << "파일 수신이 완료되었습니다" << std::endl;
            //    }
            //}
            //fclose(fp);
        }
        else if (net_events.lNetworkEvents == FD_CLOSE)
        {
            SOCKADDR_IN clientAddress = { 0 };
            GetClientAddress(clientAddress, index);
            std::cout << "[" << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "] 연결 종료" << std::endl;

            closesocket(socketArray[index]);
            WSACloseEvent(eventArray[index]);

            numOfClient--;
            socketArray[index] = socketArray[numOfClient];
            eventArray[index] = eventArray[numOfClient];
        }
    }
    closesocket(sock);
}

void Server::AddEvent(SOCKET sock, long eventType)
{
    HANDLE wsaEvent = WSACreateEvent();
    socketArray[numOfClient] = sock;
    eventArray[numOfClient] = wsaEvent;
    numOfClient++;
    WSAEventSelect(sock, wsaEvent, eventType);
}

void Server::GetClientAddress(SOCKADDR_IN& clientAddress, int index)
{
    int len = sizeof(clientAddress);
    getpeername(socketArray[index], (SOCKADDR*)&clientAddress, &len);
}
