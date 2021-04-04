#include"ClientSocket.h"
#include"UploadCommand.h"
#include"DownloadCommand.h"
#include"ListCommand.h"

ClientSocket* ClientSocket::clientSocket = nullptr;

ClientSocket::ClientSocket()
{
    AddPacketCommand(nullptr);
    AddPacketCommand(new UploadCommand);
    AddPacketCommand(new DownloadCommand);
    AddPacketCommand(new ListCommand);
}

void ClientSocket::AddPacketCommand(PacketCommand* packetCommand)
{
    packetCommands.push_back(std::unique_ptr<PacketCommand>(packetCommand));
}

ClientSocket::~ClientSocket()
{
}

ClientSocket* ClientSocket::GetInstance()
{
    if (clientSocket == nullptr)
    {
        clientSocket = new ClientSocket();
        atexit(ClientSocket::releaseInstance);
    }
    return clientSocket;
}

void ClientSocket::releaseInstance()
{
    if (clientSocket)
    {
        closesocket(clientSocket->sock);
        clientSocket->sock = INVALID_SOCKET;
        delete clientSocket;
        clientSocket = nullptr;
    }
}

SOCKET& ClientSocket::GetSocket()
{
    return sock;
}

void ClientSocket::CloseSocket()
{
    closesocket(sock);
    std::cout << "연결 종료" << std::endl;
}

void ClientSocket::Connect(int port)
{
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        exit(1);
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    memset(&serverAddress, 0, sizeof(serverAddress));

    // 서버 주소 지정
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(port);

    // 서버에 접속 요청 
    if (connect(sock, (LPSOCKADDR)&serverAddress, sizeof(serverAddress)) != 0)
    {
        std::cout << GetLastError() << std::endl;
        exit(1);
    }
    std::cout << "서버와 연결 성공" << std::endl;

    while (TRUE)
    {
        int command;
        std::cout << "1 - UPLOAD" << std::endl;
        std::cout << "2 - DOWNLOAD" << std::endl;
        std::cout << "3 - LIST" << std::endl;
        std::cout << "4 - END" << std::endl;
        std::cin >> command;

        if (command < packetCommands.size())
        {
            std::cout << "SENDCOMMAND" << std::endl;
            packetCommands.at(command)->Execute(clientSocket->GetSocket(), sendBuffer);
        }
        else
        {
            return;
        }
    }
}