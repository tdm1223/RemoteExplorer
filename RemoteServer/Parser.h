#pragma once
#include"packet.h"
#include<iostream>

class Parser
{
public:
    bool Parsing(char* recvBuffer, int byteLen, Packet& resultPacket);

private:
    int cnt_ = 0;
};
