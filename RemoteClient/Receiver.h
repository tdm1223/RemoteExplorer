#ifndef RECEIVER_H
#define RECEIVER_H

#include"PacketCommand.h"
#include<vector>
#include<memory>
#include<Windows.h>

class Receiver
{
public:
    Receiver();

    void operator()(std::vector<std::unique_ptr<PacketCommand>>* commands);
    char sendBuffer[Util::kBufferSize];
};

#endif