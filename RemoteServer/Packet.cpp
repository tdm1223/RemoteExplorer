#include "Packet.h"

Packet::Packet()
{}

Packet::~Packet()
{}

void Packet::serialize(std::vector<char>& buf)
{
    buf.push_back(prefix);
    buf.push_back(command);
    buf.push_back(htonl(size) >> 24);
    buf.push_back(htonl(size) >> 16);
    buf.push_back(htonl(size) >> 8);
    buf.push_back(htonl(size) >> 0);
    for (auto b : data)
    {
        buf.push_back(b);
    }
    buf.push_back(suffix);
}
