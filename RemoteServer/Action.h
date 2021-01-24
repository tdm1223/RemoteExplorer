#ifndef ACTION
#define ACTION
#include<string>
#include "Packet.h"

class Action
{
public:
    virtual void Execute() = 0;
};

Action* GetActionPtr(const std::string& action_str); // factory pattern

#endif