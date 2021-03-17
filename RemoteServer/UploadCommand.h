#pragma once

#include"Command.h"
#include<iostream>

class UploadCommand : public Command
{
public:
    void Execute(SOCKET& sock);
};
