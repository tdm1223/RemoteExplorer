#include"Receiver.h"
#include"PacketCommand.h"
#include<iostream>
#include"ClientSocket.h"

Receiver::Receiver()
{

}

void Receiver::operator()(std::vector<std::unique_ptr<PacketCommand>>* commands)
{
    // clientSock ���ͼ� ���
    ClientSocket* clientSock;

    while (TRUE)
    {
        // message type�� �Է� ����
        int command;
        std::cout << "1 - UPLOAD" << std::endl;
        std::cout << "2 - DOWNLOAD" << std::endl;
        std::cout << "3 - MOVE" << std::endl;
        std::cout << "4 - END" << std::endl;
        std::cin >> command;

        if (command < commands->size())
        {
            commands->at(command)->Execute(clientSock->sock);
        }
        else
        {
            std::cout << "���� ����" << std::endl;
            return;
        }
    }
}
