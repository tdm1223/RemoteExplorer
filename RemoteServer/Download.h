#pragma once
#include "Command.h"
#include<iostream>

class Download : public Command
{
    void Execute(SOCKET& sock);
};

