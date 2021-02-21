#include "Receiver.h"
#include "PacketCommand.h"
#include<iostream>

Receiver::Receiver()
{

}

void Receiver::Start()
{
}

void Receiver::operator()(std::vector<std::unique_ptr<PacketCommand>>* commands)
{
    while (true)
    {
        // recv ÈÄ 
    }
}
