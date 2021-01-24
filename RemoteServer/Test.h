#ifndef TEST_COMMAND
#define TEST_COMMAND

#include"Action.h"
#include<iostream>

class Test : public Action
{
public:
    void Execute()
    {
        std::cout << "Test start" << std::endl;
    }
};

#endif TEST_COMMAND