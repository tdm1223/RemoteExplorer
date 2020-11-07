#include "Packet.h"

Packet::Packet()
{
    command = 0;
    size = 0;
}

Packet::~Packet()
{}

void Packet::WriteWithoutData(int command, int size, int& offset)
{
    buf.push_back(prefix);
    buf.push_back(command & 0xFF);
    buf.push_back((command >> 8) & 0xFF);
    buf.push_back((command >> 16) & 0xFF);
    buf.push_back((command >> 24) & 0xFF);

    buf.push_back(size & 0xFF);
    buf.push_back((size >> 8) & 0xFF);
    buf.push_back((size >> 16) & 0xFF);
    buf.push_back((size >> 24) & 0xFF);

    offset += sizeof(char) + sizeof(int) * 2;
}

void Packet::WriteData(char* data, int size, int& offset)
{
    offset += size;
    for (int i = 0; i < size; i++)
    {
        buf.push_back(data[i]);
    }
}

void Packet::Serialize(char* data)
{
    memcpy(data, &buf[0], buf.size());
}
