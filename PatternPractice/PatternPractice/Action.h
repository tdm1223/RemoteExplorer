#pragma once
#include<string>

class Action
{
public:
    virtual void Execute() = 0;
};

Action* GetActionPtr(const std::string& action_str); // factory pattern