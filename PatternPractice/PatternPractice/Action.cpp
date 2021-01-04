#include "Action.h"
#include "Run.h"
#include "Stop.h"

Action* GetActionPtr(const std::string& action_str)
{
    if (action_str == "Run") return (Action*)new Run;
    else if (action_str == "Stop") return (Action*) new Stop;
}