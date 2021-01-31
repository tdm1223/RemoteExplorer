#include "Server.h"

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
    AddEvent(sock, FD_ACCEPT | FD_CLOSE);
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
                    AddEvent(clientSock, FD_READ | FD_CLOSE);
                    std::cout << "[" << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "] 연결 성공" << std::endl;
                }
                else if (net_events.lNetworkEvents == FD_READ)
                {
                    // RECV용 버퍼 선언 및 초기화
                    char recvBuffer[BUF_SIZE];
                    memset(recvBuffer, 0, BUF_SIZE);

                    // 버퍼 사이즈 만큼의 데이터를 가져와서 RECV용 버퍼에 저장
                    int byteLen = recv(socketArray[sigEventIdx], recvBuffer, BUF_SIZE, 0);
                    std::cout << "Server receive " << byteLen << " bytes from " << sigEventIdx << std::endl;
                    if (byteLen > 0)
                    {
                        CustomPacket packet;
                        if (!packet.OnParse(recvBuffer, byteLen))
                        {
                            continue;
                        }

                        std::cout << "클라로 부터 받은 prefix : " << packet.GetPrefix() << std::endl;
                        std::cout << "클라로 부터 받은 메시지의 타입 : " << packet.GetCommand() << std::endl;
                        std::cout << "클라로 부터 받은 메시지의 크기 : " << packet.GetSize() << std::endl;
                        std::string name;

                        int command = packet.GetCommand();
                        for (int i = 0; i < packet.GetSize(); i++)
                        {
                            name.push_back(packet.GetData()[i]);
                        }
                        std::cout << "클라로 부터 받은 메시지의 데이터 : " << name << std::endl;

                        if (commandInvoker.GetCommandFactory().count(command) > 0)
                        {
                            commandInvoker.GetCommandFactory()[command]->Execute(socketArray[index], name);
                            break;
                        }
                    }
                }
                else if (net_events.lNetworkEvents == FD_CLOSE)
                {
                    std::thread closeThread([=] {CloseProc(sigEventIdx); });
                    closeThread.join();
                }
            }
        }
    }
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

void Server::CloseProc(int index)
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