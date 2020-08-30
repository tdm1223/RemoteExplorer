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
            std::thread acceptThread([=] {AcceptProc(); });
            acceptThread.join();
        }
        else if (net_events.lNetworkEvents == FD_READ)
        {
            std::thread readThread([=] {ReadProc(index, &queue, &m, &cv); });
            readThread.join();

            cv.notify_all();

            std::thread recvThread([=] {RecvProc(&queue, &m, &cv); });
            recvThread.join();
        }
        else if (net_events.lNetworkEvents == FD_CLOSE)
        {
            std::thread closeThread([=] {CloseProc(index); });
            closeThread.join();
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

void Server::AcceptProc()
{
    SOCKADDR_IN clientAddress = { 0 };
    int len = sizeof(clientAddress);
    SOCKET sock = accept(socketArray[0], (SOCKADDR*)&clientAddress, &len);

    if (numOfClient == FD_SETSIZE)
    {
        closesocket(sock);
    }
    AddEvent(sock, FD_READ | FD_CLOSE | FD_WRITE);
    std::cout << "[" << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "] 연결 성공" << std::endl;
}

void Server::ReadProc(int num, std::queue<Files>* queue, std::mutex* m, std::condition_variable* cv)
{
    Files recvFile;
    int index = num;

    SOCKADDR_IN clientAddress = { 0 };
    GetClientAddress(clientAddress, index);

    // 파일 기본 정보를 수신
    int retval = recv(socketArray[index], (char*)&recvFile, sizeof(recvFile), 0);
    recvFile.index = index;
    if (retval == SOCKET_ERROR)
    {
        return;
    }
    std::cout << "[" << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "] 전송하는 파일 : " << recvFile.name << " 전송하는 파일 크기 : " << recvFile.size << "Byte" << std::endl;

    m->lock();
    queue->push(recvFile);
    m->unlock();

    cv->notify_one();
}

void Server::CloseProc(int num)
{
    int index = num;
    SOCKADDR_IN clientAddress = { 0 };
    GetClientAddress(clientAddress, index);
    std::cout << "[" << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "] 연결 종료" << std::endl;

    closesocket(socketArray[index]);
    WSACloseEvent(eventArray[index]);

    numOfClient--;
    socketArray[index] = socketArray[numOfClient];
    eventArray[index] = eventArray[numOfClient];
}

void Server::RecvProc(std::queue<Files>* queue, std::mutex* m, std::condition_variable* cv)
{
    std::unique_lock<std::mutex> lock(*m);
    cv->wait(lock, [&] {return !queue->empty(); });

    int retval = 0;

    Files recvFile = queue->front();
    queue->pop();
    fs::path p(recvFile.name);
    remove(p);
    lock.unlock();

    int index = recvFile.index;

    // 기존 파일 여부 확인
    FILE* fp = fopen(recvFile.name, "rb");
    int numtotal = 0;
    char buf[BUF_SIZE];
    if (fp == NULL)
    {
        // 데이터 받아서 파일 쓰는 로직
        fp = fopen(recvFile.name, "wb");
        while (1)
        {
            retval = recv(socketArray[index], buf, BUF_SIZE, 0);
            if (retval == -1)
            {
                break;
            }
            else
            {
                fwrite(buf, 1, retval, fp);
                numtotal += retval;
            }
        }
        if (numtotal == recvFile.size)
        {
            std::cout << "파일 수신이 완료되었습니다" << std::endl;
        }
    }
    fclose(fp);
}

void Server::GetClientAddress(SOCKADDR_IN& clientAddress, int index)
{
    int len = sizeof(clientAddress);
    getpeername(socketArray[index], (SOCKADDR*)&clientAddress, &len);
}
