#pragma once
#include "packet.h"
#include<iostream>

class Parser
{
public:
    bool Parse(char* data, int len, int& bytesRead, Packet& packet);

private:
    int _step = 0;
};
