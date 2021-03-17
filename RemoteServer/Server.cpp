#include"Server.h"
#include<iostream>

Server::Server()
{
    numOfClient = 0;
    WSADATA wsadata;
    commandInvoker.Initialize();

    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        exit(1);
    }
    listenSock = SetServer(); // 대기 소켓 설정

    if (listenSock == -1)
    {
        perror("대기 소켓 오류");
    }
    else
    {
        std::cout << "Create Main Thread..." << std::endl;
        EventLoop(listenSock);
    }
}

Server::~Server()
{
    WSACleanup();
}

SOCKET Server::SetServer()
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
    if (bind(sock, (sockaddr*)&servaddr, sizeof(servaddr)) == -1)
    {
        return -1;
    }

    //백 로그 큐 설정
    if (listen(sock, SOMAXCONN) == -1)
    {
        return -1;
    }
    return sock;
}

void Server::EventLoop(SOCKET sock)
{
    HANDLE newEvent = WSACreateEvent();
    socketArray[numOfClient] = sock;
    eventArray[numOfClient] = newEvent;
    numOfClient++;
    WSAEventSelect(sock, newEvent, FD_ACCEPT | FD_CLOSE);

    while (true)
    {
        // 이벤트 발생을 기다리면서 가장 처음 발생한 index를 반환
        int index = WSAWaitForMultipleEvents(numOfClient, eventArray, false, INFINITE, false);
        int startIndex = index - WSA_WAIT_EVENT_0;

        for (int i = startIndex; i < numOfClient; i++)
        {
            int sigEventIdx = WSAWaitForMultipleEvents(1, &eventArray[i], TRUE, 0, FALSE);

            if (sigEventIdx == WSA_WAIT_FAILED || sigEventIdx == WSA_WAIT_TIMEOUT)
            {
                continue;
            }
            else
            {
                sigEventIdx = i;
                WSANETWORKEVENTS net_events;

                // 이벤트 종류 알아내기
                WSAEnumNetworkEvents(socketArray[sigEventIdx], eventArray[sigEventIdx], &net_events);
                if (net_events.lNetworkEvents == FD_ACCEPT)
                {
                    SOCKADDR_IN clientAddress = { 0 };
                    int len = sizeof(clientAddress);
                    clientSock = accept(socketArray[sigEventIdx], (SOCKADDR*)&clientAddress, &len);

                    if (numOfClient == FD_SETSIZE)
                    {
                        closesocket(clientSock);
                    }

                    threadPool->EnqueueJob([&]() {AddEvent(clientSock, FD_READ | FD_CLOSE); });
                    std::cout << "[" << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "] 연결 성공" << std::endl;
                }
                else if (net_events.lNetworkEvents == FD_READ)
                {
                    // 스레드 풀에 해당 작업을 추가함
                    // RECV용 버퍼 선언 및 초기화
                    char recvBuffer[kBufferSize];
                    memset(recvBuffer, 0, kBufferSize);
                    // 버퍼 사이즈 만큼의 데이터를 가져와서 RECV용 버퍼에 저장
                    if (recv(socketArray[sigEventIdx], recvBuffer, kBufferSize, 0) > 0)
                    {
                        // prefix와 command를 읽음
                        int offset = 0;

                        // prefix check
                        char prefix[8] = "";
                        memcpy(prefix, recvBuffer, sizeof(char) * 8);
                        offset += sizeof(char) * 8;
                        std::cout << prefix << std::endl;

                        int command = 0;
                        memcpy(&command, recvBuffer + offset, sizeof(int));
                        offset += sizeof(int);
                        std::cout << command << std::endl;

                        // 아마 length는 받지 않고 command만 받아서 Execute 인자에 버퍼의 나머지 부분을 넘겨주도록 구현해야 할 것 같다.
                        // Execute(sock, msg, msgLen); 이런식으로?
                        // Execute 함수에서는 msg, msgLen 만큼 다시 읽어서 처리한다.
                        // 모든 패킷은 prefix, command (length, data) 이런식으로 만들어진다.
                        int length = 0;
                        memcpy(&length, recvBuffer + offset, sizeof(int));
                        offset += sizeof(int);
                        std::cout << length << std::endl;

                        if (length > 0)
                        {
                            memset(message, 0, 4096);
                            memcpy(message, recvBuffer + offset, length);
                            printf("%s\n", message);
                        }

                        // 스레드풀에 실행명령을 넣음
                        threadPool->EnqueueJob([&]() {commandInvoker.GetCommandFactory()[command]->Execute(socketArray[sigEventIdx]); });
                    }
                }
                else if (net_events.lNetworkEvents == FD_CLOSE)
                {
                    threadPool->EnqueueJob([&]() {CloseProc(sigEventIdx, numOfClient); });
                }
            }
        }
    }

    // 서버 소켓 종료
    closesocket(sock);
}

void Server::AddEvent(SOCKET sock, long eventType)
{
    HANDLE newEvent = WSACreateEvent();
    socketArray[numOfClient] = sock;
    eventArray[numOfClient] = newEvent;
    numOfClient++;
    WSAEventSelect(sock, newEvent, eventType);
}

void Server::GetClientAddress(SOCKADDR_IN& clientAddress, int index)
{
    int len = sizeof(clientAddress);
    getpeername(socketArray[index], (SOCKADDR*)&clientAddress, &len);
}

void Server::CloseProc(int index, int& numOfClient)
{
    SOCKADDR_IN clientAddress = { 0 };

    GetClientAddress(clientAddress, index);
    int len = sizeof(clientAddress);
    getpeername(socketArray[index], (SOCKADDR*)&clientAddress, &len);

    std::cout << "[" << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "] 연결 종료" << std::endl;

    closesocket(socketArray[index]);
    WSACloseEvent(eventArray[index]);

    numOfClient--;
    socketArray[index] = socketArray[numOfClient];
    eventArray[index] = eventArray[numOfClient];
}