#pragma once

#include"Action.h"
#include<iostream>

using namespace std;

class Send : public Action
{
public:
    void Execute()
    {
        cout << "Send" << endl;
    }
};