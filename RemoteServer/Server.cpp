#include"Server.h"
#include"Util.h"
#include<iostream>

Server::Server()
{
    // 프로세서 개수를 구한다.
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    numberOfThreads_ = systemInfo.dwNumberOfProcessors * 2;
    threadPool = std::make_unique<ThreadPool>(numberOfThreads_);
    std::cout << "생성된 스레드 수 : " << numberOfThreads_ << std::endl;
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
    servaddr.sin_port = htons(Util::kPort);

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
                    char recvBuffer[Util::kBufferSize];
                    memset(recvBuffer, 0, Util::kBufferSize);

                    // 버퍼 사이즈 만큼의 데이터를 가져와서 RECV용 버퍼에 저장
                    if (recv(socketArray[sigEventIdx], recvBuffer, Util::kBufferSize, 0) == SOCKET_ERROR)
                    {
                        continue;
                    }

                    int offset = 0;

                    // read and check prefix
                    char prefix[Util::kPrefixSize] = "";
                    memcpy(prefix, recvBuffer, Util::kPrefixSize);
                    offset += Util::kPrefixSize;
                    if (strcmp(prefix, "ESTSOFT") != 0)
                    {
                        continue;
                    }

                    // read and check command
                    int command = 0;
                    memcpy(&command, recvBuffer + offset, Util::kCommandSize);
                    offset += Util::kCommandSize;
                    std::cout << "COMMAND : " << command << std::endl;

                    // read and check length
                    int length = 0;
                    memcpy(&length, recvBuffer + offset, Util::kLengthSize);
                    offset += Util::kLengthSize;

                    // read and check data
                    char data[Util::kBufferSize];
                    memset(data, 0, Util::kBufferSize);
                    if (length > 0)
                    {
                        memcpy(data, recvBuffer + offset, length);
                    }

                    // 스레드풀에 실행명령을 넣음
                    threadPool->EnqueueJob([&]() {commandInvoker.GetCommandFactory()[command]->Execute(socketArray[sigEventIdx], data); });
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