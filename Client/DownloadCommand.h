#pragma once
#include<iostream>
#include "PacketCommand.h"

class DownloadCommand : public PacketCommand
{
    bool Execute();
};