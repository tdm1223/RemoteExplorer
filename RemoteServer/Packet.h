#pragma once
#include<vector>
#include<winsock.h>
#include<iostream>

#define BUF_SIZE 4096
class Packet
{
public:
    Packet();
    ~Packet();

    void WriteWithoutData(int command, int size, int& offset);
    void WriteData(char* data, int size, int& offset);
    void Serialize(char* data);
    char prefix = 0x7F;
    int command;
    int size;

    std::vector<char> buf;
};