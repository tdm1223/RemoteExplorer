#include"IOCPServer.h"
#include"Util.h"
#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>

IOCPServer::IOCPServer()
{
    // ���μ��� ������ ���Ѵ�.
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    numberOfThreads_ = systemInfo.dwNumberOfProcessors * 2;
    isServerRunning_ = false;
}

void IOCPServer::WorkerThreadEntry_(void* iocpserver)
{
    reinterpret_cast<IOCPServer*>(iocpserver)->WorkerThread_();
}

IOCPServer::~IOCPServer()
{
    if (!isServerRunning_)
    {
        SetEvent(shutdownEvent_);

        acceptThread_->join();
        delete acceptThread_;

        WSACloseEvent(acceptEvent_);

        for (int i = 0; i < numberOfThreads_; i++)
        {
            PostQueuedCompletionStatus(completionPortHandle_, 0, NULL, NULL);
        }

        for (auto thread : workerThreads_)
        {
            thread->join();
            delete thread;
        }

        CloseHandle(shutdownEvent_);

        workerThreads_.clear();

        CloseHandle(completionPortHandle_);
        closesocket(listenSocket_);

        ResetEvent(shutdownEvent_);
        WSACleanup();

        ResetEvent(shutdownEvent_);
        WSACleanup();
        isServerRunning_ = false;
    }
}

bool IOCPServer::Start()
{
    std::cout << "������ ������ �� : " << numberOfThreads_ << std::endl;

    WSADATA wsadata;
    commandInvoker.Initialize();

    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        std::cout << "WSAStartup ERROR" << std::endl;
        return false;
    }

    if (!InitializeIOCP_())
    {
        std::cout << "InitializeIOCP_ ERROR" << std::endl;
    }

    listenSocket_ = WSASocket(AF_INET, SOCK_STREAM, 0, nullptr, 0, WSA_FLAG_OVERLAPPED);
    if (listenSocket_ == INVALID_SOCKET)
    {
        std::cout << "Start(): WSASocket ERROR!" << std::endl;
        return false;
    }

    SOCKADDR_IN socketAddress;
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.S_un.S_addr = INADDR_ANY;
    socketAddress.sin_port = htons(Util::kPort);

    if (bind(listenSocket_, reinterpret_cast<SOCKADDR*>(&socketAddress), sizeof(socketAddress)) == SOCKET_ERROR)
    {
        std::cout << "Start(): bind ERROR!" << std::endl;
        return false;
    }

    if (listen(listenSocket_, 5) == SOCKET_ERROR)
    {
        std::cout << "Start(): listen ERROR!" << std::endl;
        return false;
    }

    acceptEvent_ = WSACreateEvent();
    if (acceptEvent_ == WSA_INVALID_EVENT)
    {
        std::cout << "Start(): WSACreateEvent ERROR!" << std::endl;
        return false;
    }

    if (WSAEventSelect(listenSocket_, acceptEvent_, FD_ACCEPT))
    {
        std::cout << "Start(): WSAEventSelect ERROR!" << std::endl;
        return false;
    }

    SECURITY_ATTRIBUTES* noSecurityAttributes = nullptr;
    shutdownEvent_ = CreateEvent(noSecurityAttributes, TRUE, FALSE, nullptr);
    if (shutdownEvent_ == nullptr)
    {
        return false;
    }

    isServerRunning_ = true;
    acceptThread_ = new std::thread(AcceptThreadEntry_, this);
    std::cout << "Start(): Server has Started." << std::endl;
    return true;
}

bool IOCPServer::InitializeIOCP_()
{
    bool returnValue = false;
    completionPortHandle_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

    if (completionPortHandle_)
    {
        workerThreads_.reserve(numberOfThreads_);
        for (int i = 0; i < numberOfThreads_; i++)
        {
            workerThreads_.push_back(new std::thread(WorkerThreadEntry_, this));
        }

        returnValue = true;
    }
    return returnValue;
}

void IOCPServer::WorkerThread_()
{
    DWORD bytesTransferred, flags;
    OVERLAPPED* overlapped;
    NetworkSession* session;

    while (WaitForSingleObject(shutdownEvent_, 0) != WAIT_OBJECT_0)
    {
        int success = GetQueuedCompletionStatus(
            completionPortHandle_,
            &bytesTransferred,
            reinterpret_cast<PULONG_PTR>(&session),
            &overlapped,
            INFINITE
        );

        if (overlapped == nullptr || session == nullptr)
        {
            break;
        }

        NetworkContext* context = new NetworkContext(NetworkContext::OperationType::kRecv);
        context->SetOverlappedPointer(overlapped);
        if (!success || (success && bytesTransferred == 0))
        {
            std::cout << "Ŭ���̾�Ʈ ���� ����" << std::endl;
            closesocket(session->GetSocket());
            delete session;
            delete context;
            continue;
        }

        std::cout << "Ŭ���̾�Ʈ�κ��� ��û�� �� " << (int)context->GetOperationType() << std::endl;
        switch (context->GetOperationType())
        {
        case (NetworkContext::OperationType::kSend):
            std::cout << "����" << std::endl;
            delete context;
            break;

        case (NetworkContext::OperationType::kLength):
        {
            int32_t length = NetworkContext::DeserializeInt(context->GetWsaBuffer());
            std::cout << "���� ���� : " << length << std::endl;

            context->CleanContext();

            context->SetWsaBufferLength(length);
            context->SetOperationType(NetworkContext::OperationType::kRecv);
            flags = 0;
            WSARecv(session->GetSocket(), context->GetWsaBufferPointer(), 1, NULL, &flags, context->GetOverlappedPointer(), NULL);
            break;
        }
        case (NetworkContext::OperationType::kRecv):
        {
            std::cout << session->GetIpAddress() << std::endl;
            std::cout << context->GetWsaBuffer() << std::endl;
            char tempBuffer[NetworkContext::kMaxBufferSize] = { 0 };
            strcpy_s(tempBuffer, 5, "NONE");
            Send_(session, tempBuffer, 5);
            SendLength_(session, NetworkContext::kStopSignalCode);

            // ���� �簳
            context->CleanContext();
            context->SetOperationType(NetworkContext::OperationType::kLength);
            flags = 0;
            WSARecv(session->GetSocket(), context->GetWsaBufferPointer(), 1, NULL, &flags, context->GetOverlappedPointer(), NULL);
            break;
        }
        }
    }
}

void IOCPServer::AcceptThread_()
{
    WSANETWORKEVENTS networkEvents;

    while (isServerRunning_ && (WaitForSingleObject(shutdownEvent_, 0) != WAIT_OBJECT_0))
    {
        if (WSAWaitForMultipleEvents(1, &acceptEvent_, FALSE, 100, FALSE) != WSA_WAIT_TIMEOUT)
        {
            WSAEnumNetworkEvents(listenSocket_, acceptEvent_, &networkEvents);
            if ((networkEvents.lNetworkEvents & FD_ACCEPT) && (networkEvents.iErrorCode[FD_ACCEPT_BIT] == 0))
            {
                AcceptConnection_();
            }
        }
    }
}

void IOCPServer::AcceptConnection_()
{
    SOCKET clientSocket;
    SOCKADDR_IN clientAddress;
    int addressSize = sizeof(clientAddress);
    clientSocket = accept(listenSocket_, reinterpret_cast<SOCKADDR*>(&clientAddress), &addressSize);
    if (clientSocket != INVALID_SOCKET)
    {
        char ipAddress[14];
        InetNtopA(clientAddress.sin_family, &clientAddress.sin_addr, ipAddress, 14);
        std::cout << "[" << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "] ���� ����" << std::endl;
        NetworkSession* session = new NetworkSession(clientSocket, clientAddress);

        HANDLE ret = CreateIoCompletionPort(
            reinterpret_cast<HANDLE>(clientSocket),
            completionPortHandle_,
            reinterpret_cast<size_t>(session),
            numberOfThreads_
        );
        if (ret != NULL)
        {
            NetworkContext* context = new NetworkContext(NetworkContext::OperationType::kLength);

            int receiveBytes, flags;
            flags = 0;
            WSARecv(
                session->GetSocket(),
                context->GetWsaBufferPointer(),
                1,
                reinterpret_cast<DWORD*>(&receiveBytes),
                reinterpret_cast<DWORD*>(&flags),
                context->GetOverlappedPointer(),
                NULL
            );
        }
        else
        {
            delete session;
        }
    }
}

void IOCPServer::AcceptThreadEntry_(void* iocpserver)
{
    reinterpret_cast<IOCPServer*>(iocpserver)->AcceptThread_();
}

int32_t IOCPServer::Send_(NetworkSession* session, char* message, int32_t length)
{
    bool succeed = false;
    DWORD recvBytes = 0;

    // �޼��� ���̸� ���� ����
    if (!SendLength_(session, length))
    {
        return false;
    }
    std::cout << "������ ���� ����" << std::endl;
    NetworkContext* request = new NetworkContext(NetworkContext::OperationType::kSend);
    std::cout << message << std::endl;
    request->SetBuffer(message, length);

    if (WSASend(session->GetSocket(), request->GetWsaBufferPointer(), 1, &recvBytes, 0, request->GetOverlappedPointer(), NULL) == SOCKET_ERROR)
    {
        int32_t error = WSAGetLastError();
        if (error != WSA_IO_PENDING)
        {
            delete request;
            return false;
        }
    }
    else
    {
        std::cout << "������ ����" << std::endl;
    }
    succeed = true;
    return true;
}

bool IOCPServer::SendLength_(NetworkSession* session, int32_t length)
{
    bool succeed = false;

    NetworkContext* request = new NetworkContext(NetworkContext::OperationType::kSend);

    // �޼��� 4����Ʈ�� ���̸� ��� ����
    char buffer[4] = { 0 };
    NetworkContext::SerializeInt(length, buffer);

    request->SetBuffer(buffer, 4);
    DWORD sendBytes;

    if (WSASend(session->GetSocket(), request->GetWsaBufferPointer(), 1, &sendBytes, 0, request->GetOverlappedPointer(), NULL) == SOCKET_ERROR)
    {
        int32_t error = WSAGetLastError();
        if (error != WSA_IO_PENDING)
        {
            delete request;
            return false;
        }
    }

    succeed = true;
    return true;
}