#include "Receiver.h"
#include "PacketCommand.h"
#include<iostream>

Receiver::Receiver()
{

}

void Receiver::operator()(std::vector<std::unique_ptr<PacketCommand>>* commands)
{
    while (TRUE)
    {
        // message type을 입력 받음
        int command;
        std::cout << "1 - UPLOAD" << std::endl;
        std::cout << "2 - DOWNLOAD" << std::endl;
        std::cout << "3 - END" << std::endl;
        std::cin >> command;

        commands->at(command)->Execute();
    }
}
