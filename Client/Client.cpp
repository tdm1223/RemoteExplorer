#include "Client.h"
#include<thread>
Client::Client()
{
    std::cout << "클라 생성자" << std::endl;
    std::thread clientThread(receiver, &commands);
    clientThread.detach();
}

void Client::AddPacketCommand(PacketCommand* command)
{
    commands.push_back(std::unique_ptr<PacketCommand>(command));
}