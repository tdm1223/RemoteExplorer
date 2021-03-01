#include"Receiver.h"
#include"PacketCommand.h"
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
        std::cout << "3 - MOVE" << std::endl;
        std::cout << "4 - END" << std::endl;
        std::cin >> command;

        if (command < commands->size())
        {
            commands->at(command)->Execute();
        }
        else
        {
            std::cout << "범위 오류" << std::endl;
            return;
        }
    }
}
