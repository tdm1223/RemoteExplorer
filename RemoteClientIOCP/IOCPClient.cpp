#include<iostream>
#include"NetworkContext.h"
#include"NetworkSession.h"
#include"IOCPClient.h"
#include"DownloadCommand.h"
#include"ListCommand.h"
#include"EchoCommand.h"

IOCPClient::IOCPClient() : isConnected_(false)
{
    hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 1);
    AddPacketCommand(nullptr);
    AddPacketCommand(new EchoCommand);
    AddPacketCommand(new DownloadCommand);
    AddPacketCommand(new ListCommand);
}

void IOCPClient::AddPacketCommand(PacketCommand* packetCommand)
{
    packetCommands.push_back(std::unique_ptr<PacketCommand>(packetCommand));
}

IOCPClient::~IOCPClient()
{
    if (isConnected_)
    {
        Disconnect();
    }
}

bool IOCPClient::Connect(const std::string& ip, const int port)
{
    if (isConnected_)
    {
        return false;
    }

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        return false;
    }

    connectSocket_ = socket(PF_INET, SOCK_STREAM, 0);
    if (connectSocket_ == INVALID_SOCKET)
    {
        return false;
    }

    SOCKADDR_IN servserAddress;
    servserAddress.sin_family = AF_INET;
    servserAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    servserAddress.sin_port = htons(port);

    NetworkSession* session = new NetworkSession(connectSocket_, servserAddress);
    CreateIoCompletionPort((HANDLE)connectSocket_, hCompletionPort, (DWORD)session, 0);

    if (connect(connectSocket_, reinterpret_cast<SOCKADDR*>(&servserAddress), sizeof(servserAddress)) == SOCKET_ERROR)
    {
        int32_t error = WSAGetLastError();
        printf("CONNECT ERROR %d\n", error);
        return false;
    }

    isConnected_ = true;
    return true;
}

bool IOCPClient::Disconnect()
{
    if (isConnected_)
    {
        isConnected_ = false;
        closesocket(connectSocket_);
        WSACleanup();
    }

    return true;
}

bool IOCPClient::Query(int command)
{
    char buffer[4096];
    if (command < packetCommands.size())
    {
        packetCommands.at(command)->Execute(connectSocket_, buffer);
        return true;
    }
    else
    {
        return false;
    }
}

bool IOCPClient::Query(const char* message)
{
    if (Send_(message))
    {
        char buffer[NetworkContext::kMaxBufferSize];
        int size;
        while (Recv_(buffer, &size))
        {
            ParseResponse_(buffer, size);
        }
        return true;
    }

    return false;
}

void IOCPClient::ParseResponse_(char* message, int32_t length) const
{
    if (strcmp(message, "NONE") != 0)
    {
        char* pointer = message;
        std::cout << length << std::endl;
    }
    std::cout << "종료" << std::endl;
}

bool IOCPClient::Send_(const char* message)
{
    // 메세지 길이를 먼저 전송
    if (!SendLength_(static_cast<int32_t>(strlen(message))))
    {
        return false;
    }
    if (send(connectSocket_, message, static_cast<int32_t>(strlen(message)), 0) == SOCKET_ERROR)
    {
        return false;
    }
    return true;
}

bool IOCPClient::SendLength_(int32_t length)
{
    // 메세지 4바이트에 길이를 담아 보냄
    char buffer[NetworkContext::kLengthMessageSize] = { 0 };
    NetworkContext::SerializeInt(length, buffer);

    if (send(connectSocket_, buffer, NetworkContext::kLengthMessageSize, 0) == SOCKET_ERROR)
    {
        return false;
    }

    return true;
}

bool IOCPClient::Recv_(char* outputString, int32_t* size)
{
    int32_t length;
    RecvLength_(&length);
    if (length == 0 || length == NetworkContext::kStopSignalCode)
    {
        return false;
    }

    char buffer[NetworkContext::kMaxBufferSize];

    DWORD flags = 0;
    int32_t receivedBytes = 0;
    int32_t totalBytes = length;
    char* bufferPointer = buffer;
    do
    {
        receivedBytes = recv(connectSocket_, bufferPointer, totalBytes, flags);
        if (receivedBytes == SOCKET_ERROR)
        {
            return false;
        }
        totalBytes -= receivedBytes;
        bufferPointer += receivedBytes;
    } while (totalBytes > 0);

    memcpy_s(outputString, length, buffer, length);
    outputString[length] = '\0';
    if (size != nullptr)
    {
        *size = length;
    }
    return true;
}

bool IOCPClient::RecvLength_(int32_t* outputInt)
{
    bool succeed = false;

    char buffer[NetworkContext::kLengthMessageSize];

    DWORD flags = 0;
    if (recv(connectSocket_, buffer, NetworkContext::kLengthMessageSize, flags) == SOCKET_ERROR)
    {
        return false;
    }

    // 메세지의 첫번째 Byte가 길이
    *outputInt = NetworkContext::DeserializeInt(buffer);

    succeed = true;
    return true;
}
