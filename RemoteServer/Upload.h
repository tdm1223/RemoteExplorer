#pragma once

#include"Command.h"
#include<iostream>

class Upload : public Command
{
public:
    void Execute(SOCKET& sock);
};
