#ifndef ACTION
#define ACTION
#include<string>
#include "Packet.h"

class Action
{
public:
    virtual void Execute(SOCKET& sock) = 0;
    CustomPacket packet;
    const int bufSize = 4096;
};

Action* GetAction(const std::string& action_str); // factory pattern

#endif