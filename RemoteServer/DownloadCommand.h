#pragma once
#include "Command.h"
#include<iostream>

class DownloadCommand : public Command
{
    void Execute(SOCKET& sock);
};

