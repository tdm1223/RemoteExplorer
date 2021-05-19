#include"Client.h"
#include<iostream>

Client::Client()
{
    memset(sendBuffer, 0, Util::kBufferSize);
    clientSocket = nullptr;

    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    numberOfThreads_ = systemInfo.dwNumberOfProcessors * 2;
    threadPool = std::make_unique<ThreadPool>(numberOfThreads_);
    std::cout << "생성된 스레드 수 : " << numberOfThreads_ << std::endl;
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
        std::cout << "1 - ECHO" << std::endl;
        std::cout << "2 - DOWNLOAD" << std::endl;
        std::cout << "3 - LIST" << std::endl;
        std::cout << "4 - END" << std::endl;
        std::cin >> command;

        memset(sendBuffer, 0, sizeof(Util::kBufferSize));
        if (command < clientSocket->packetCommands.size())
        {
            if (command == Util::COMMAND::DOWNLOAD || command == Util::COMMAND::ECHO)
            {
                std::cout << "input data : ";
                std::string data;
                std::cin >> data;
                memcpy(sendBuffer, data.c_str(), data.size());
            }
            threadPool->EnqueueJob([&]() {clientSocket->packetCommands.at(command)->Execute(clientSocket->GetSocket(), sendBuffer); });
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