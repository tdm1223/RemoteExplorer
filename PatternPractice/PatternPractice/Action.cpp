#include "Action.h"
#include "Receive.h"
#include "Send.h"

Action* GetActionPtr(const std::string& action_str)
{
    if (action_str == "Receive") return (Action*)new Receive;
    else if (action_str == "Send") return (Action*) new Send;
}