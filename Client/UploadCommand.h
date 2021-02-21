#pragma once

#include<iostream>
#include "PacketCommand.h"
class UploadCommand : public PacketCommand
{
    bool Execute();
};