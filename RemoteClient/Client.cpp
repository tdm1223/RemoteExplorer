﻿#include"Client.h"
#include<thread>

Client::Client()
{
    memset(sendBuffer, 0, Util::kBufferSize);
    clientSocket = nullptr;
}

#include<iostream>
bool Client::Initialize()
{
    clientSocket = ClientSocket::GetInstance();
    if (clientSocket->Connect(kPort))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void Client::Loop()
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
            std::cout << "SENDCOMMAND" << std::endl;
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