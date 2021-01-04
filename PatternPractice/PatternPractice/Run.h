#pragma once

#include "Action.h"
#include<iostream>

using namespace std;

class Run : public Action
{
public:
    void Execute()
    {
        cout << "Run" << endl;
    }
};