#pragma once
#include<vector>
#include<winsock.h>

class Packet
{
public:
    Packet();
    ~Packet();

    void serialize(std::vector<char>& buf);

    unsigned int command;
    unsigned int size;
};