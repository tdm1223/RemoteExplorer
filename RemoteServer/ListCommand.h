#pragma once

#include"Command.h"
#include<iostream>

class ListCommand : public Command
{
public:
    void Execute(SOCKET& sock);
};
