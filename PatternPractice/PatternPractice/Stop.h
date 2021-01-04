#pragma once
#include "Action.h"
#include<iostream>

using namespace std;

class Stop : public Action
{
public:
    void Execute()
    {
        cout << "Stop" << endl;
    }
};