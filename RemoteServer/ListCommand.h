#ifndef SERVER_LIST_COMMAND_H
#define SERVER_LIST_COMMAND_H

#include"Command.h"
#include<iostream>
#include<filesystem>

namespace fs = std::filesystem;

class ListCommand : public Command
{
public:
    bool Execute(SOCKET sock, char* buf);
};

#endif // SERVER_LIST_COMMAND_H