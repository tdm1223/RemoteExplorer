#include"Client.h"
#include<iostream>

Client::Client()
{
    memset(sendBuffer, 0, Util::kBufferSize);
    clientSocket = nullptr;
}

bool Client::Initialize()
{
    clientSocket = ClientSocket::GetInstance();
    if (clientSocket->Connect(Util::kPort))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Client::Start()
{
    while (TRUE)
    {
        int command;
        std::cout << "1 - UPLOAD" << std::endl;
        std::cout << "2 - DOWNLOAD" << std::endl;
        std::cout << "3 - LIST" << std::endl;
        std::cout << "4 - END" << std::endl;
        std::cin >> command;

        if (command < clientSocket->packetCommands.size())
        {
            clientSocket->packetCommands.at(command)->Execute(clientSocket->GetSocket(), sendBuffer);
        }
        else
        {
            Close();
            return;
        }
    }
}

void Client::Close()
{
    clientSocket->CloseSocket();
}