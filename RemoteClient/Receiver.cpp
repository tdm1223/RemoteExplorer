#include"Receiver.h"
#include"PacketCommand.h"
#include<iostream>
#include"ClientSocket.h"

Receiver::Receiver()
{
}

void Receiver::operator()(std::vector<std::unique_ptr<PacketCommand>>* commands)
{
    ClientSocket* clientSocket = ClientSocket::GetInstance();

    while (TRUE)
    {
        int command;
        std::cout << "1 - UPLOAD" << std::endl;
        std::cout << "2 - DOWNLOAD" << std::endl;
        std::cout << "3 - LIST" << std::endl;
        std::cout << "4 - END" << std::endl;
        std::cin >> command;

        if (command < commands->size())
        {
            commands->at(command)->Execute(clientSocket->GetSocket(), sendBuffer);
        }
        else
        {
            return;
        }
    }
}
