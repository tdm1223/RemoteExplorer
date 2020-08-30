#include "Server.h"

Server::Server()
{
    numOfClient = 0;
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        exit(1);
    }
    SOCKET sock = SetServer(ServerSize); // ��� ���� ����
    if (sock == -1)
    {
        perror("��� ���� ����");
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

    // ���� ���� ����
    SOCKADDR_IN servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // ���� �ּҿ� ��Ʈ��ũ �������̽� ����
    if (bind(sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
    {
        return -1;
    }

    //�� �α� ť ����
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
        // �̺�Ʈ �߻��� ��ٸ��鼭 ���� ó�� �߻��� index�� ��ȯ
        int index = WSAWaitForMultipleEvents(numOfClient, eventArray, false, INFINITE, false);
        WSANETWORKEVENTS net_events;

        // �̺�Ʈ ���� �˾Ƴ���
        WSAEnumNetworkEvents(socketArray[index], eventArray[index], &net_events);
        if (net_events.lNetworkEvents == FD_ACCEPT)
        {
            std::thread acceptThread([=] {AcceptProc(); });
            acceptThread.join();
        }
        else if (net_events.lNetworkEvents == FD_READ)
        {
            std::thread readThread([=] {ReadProc(index); });
            readThread.join();
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
    std::cout << "[" << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "] ���� ����" << std::endl;
}

void Server::ReadProc(int num)
{
    Files recvFile;
    int index = num;

    SOCKADDR_IN clientAddress = { 0 };
    GetClientAddress(clientAddress, index);

    // ���� �⺻ ������ ����
    int retval = recv(socketArray[index], (char*)&recvFile, sizeof(recvFile), 0);
    if (retval == SOCKET_ERROR)
    {
        return;
    }
    std::cout << "[" << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "] �����ϴ� ���� : " << recvFile.name << " �����ϴ� ���� ũ�� : " << recvFile.size << "Byte" << std::endl;

    // ���� ���� ���� Ȯ��
    FILE* fp = fopen(recvFile.name, "rb");
    int numtotal = 0;
    char buf[BUF_SIZE];
    if (fp == NULL)
    {
        // ������ �޾Ƽ� ���� ���� ����
        fp = fopen(recvFile.name, "wb");
        std::cout << "���ϸ��� ���� ������ �������� �ʽ��ϴ�" << std::endl;
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
            std::cout << "���� ������ �Ϸ�Ǿ����ϴ�" << std::endl;
        }
    }
    else
    {
        std::cout << "���ϸ��� ���� ������ �����մϴ� ���� �������� �ʽ��ϴ�." << std::endl;
        while (1)
        {
            retval = recv(socketArray[index], buf, BUF_SIZE, 0);
            if (retval == -1)
            {
                break;
            }
            else
            {
                numtotal += retval;
            }
        }
    }

    fclose(fp);
    return;
}

void Server::CloseProc(int num)
{
    int index = num;
    SOCKADDR_IN clientAddress = { 0 };
    GetClientAddress(clientAddress, index);
    std::cout << "[" << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "] ���� ����" << std::endl;

    closesocket(socketArray[index]);
    WSACloseEvent(eventArray[index]);

    numOfClient--;
    socketArray[index] = socketArray[numOfClient];
    eventArray[index] = eventArray[numOfClient];
}

void Server::GetClientAddress(SOCKADDR_IN& clientAddress, int index)
{
    int len = sizeof(clientAddress);
    getpeername(socketArray[index], (SOCKADDR*)&clientAddress, &len);
}
