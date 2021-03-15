#pragma once

#include"Command.h"
#include<iostream>

class Move : public Command
{
public:
    void Execute(SOCKET& sock);
};
