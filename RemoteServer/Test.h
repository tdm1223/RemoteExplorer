#ifndef TEST_COMMAND
#define TEST_COMMAND

#include"Action.h"
#include<iostream>

class Test : public Action
{
public:
    void Execute(SOCKET& sock);
};

#endif TEST_COMMAND