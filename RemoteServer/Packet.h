#pragma once
#include<vector>
#include<winsock.h>
#include<iostream>

class Pakcet
{
public:
    virtual const bool OnParse(const char* buffer, const unsigned int bufferSize) = 0;
    virtual const bool OnBuild(char* buffer, unsigned int& buildBufferSize) = 0;
};

class MyPacket : public Packet
{
public:
    const int GetCommand() const
    {
        return command;
    }

    virtual const bool OnParse(const char* buffer, const unsigned int bufferSize) override
    {
        if (buffer == nullptr || bufferSize < header + body)
        {
            return false;
        }

       //...
        command = 1;

        //...
        size = 2;

        return true;
    }

    virtual const bool OnBuild(char* buffer, unsigned int& buildBufferSize) override
    {
        if (command > command validate || size > size validate)
        {
            return false
        }

        memcpy(buffer, command, 4);
        memcpy(buffer + 4, size, 4);


        buildbufferSize = 8;

        return true;
    }

private:
    int command;
    int size;
};

server code
{
    //case Send
    MyPacket packet;
   packet.command = 4;
    packet.size = 8;

    char pakcetBuffer[MAX_PACKET] = { 0, };
    uint32_t buildBufferSize = 0;
    if (packet.OnBuild(packetBuffer, buildBufferSize) == true)
    {
        Send(socket, packetBuffer, buildBufferSize);
    }

    //case Recv
    MyPacket packet;
    if (packet.OnParse(recvBuffer, 8) == true)
    {
        packet.GetCommand();
    }

}

class Packet
{
public:
    Packet();
    ~Packet();

    void WriteWithoutData(int command, int size, int& offset);
    void WriteData(char* data, int size, int& offset);
    void Serialize(char* data);
    void Build(char* buffer, int command, int size, char* data, int& offset);
    int GetHeaderSize();
    char prefix = 0x7F;
    int command;
    int size;

    std::vector<char> buf;
};