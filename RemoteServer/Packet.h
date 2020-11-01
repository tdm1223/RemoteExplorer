#pragma once
#include<vector>
#include<winsock.h>

class Packet
{
public:
    Packet();
    ~Packet();

    void serialize(std::vector<char>& buf);

    char prefix = 0x7F;
    char command = '0';
    int size = 0;
    std::vector<char> data;
    char suffix = 0x7F;
};