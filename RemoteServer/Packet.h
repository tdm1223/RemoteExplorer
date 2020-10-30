#pragma once
#include<vector>
#include<winsock.h>

class Packet
{
public:
    Packet();
    ~Packet();

    void serialize(std::vector<char>& buf);

    char prefix = 0x7F; // Marks the begin of `Packet`
    char command = '0'; // Identifies the packet's type - what is it used for.
    int size = 0; // Size of `data`.
    std::vector<char> data; // Custom data.
    char suffix = 0x7F;
};