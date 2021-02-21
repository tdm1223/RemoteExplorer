#include "Client.h"
#include<thread>
#include"UploadCommand.h"
#include"DownloadCommand.h"
#include"MoveCommand.h"

Client::Client()
{
    AddPacketCommand(nullptr);
    AddPacketCommand(new UploadCommand);
    AddPacketCommand(new DownloadCommand);
    AddPacketCommand(new MoveCommand);
    // 소켓을 초기화
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        exit(1);
    }

    // client 소켓 생성
    clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (clientSock == INVALID_SOCKET)
    {
        WSACleanup();
        exit(1);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));

    // 서버 주소 지정
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(PORT);

    // 서버에 접속 요청 
    if (connect(clientSock, (LPSOCKADDR)&serverAddress, sizeof(serverAddress)) != 0)
    {
        exit(1);
    }
}

void Client::Initialize()
{
    Receiver receiver;
    std::thread commandThread(receiver, &commands);
    commandThread.join();

    // 소켓을 닫음
    closesocket(clientSock);
    std::cout << "client exit" << std::endl;
}

void Client::AddPacketCommand(PacketCommand* command)
{
    commands.push_back(std::unique_ptr<PacketCommand>(command));
}