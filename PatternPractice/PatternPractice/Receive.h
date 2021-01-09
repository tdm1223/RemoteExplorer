#pragma once

#include"Action.h"
#include<iostream>

using namespace std;

class Receive : public Action
{
public:
    void Execute()
    {
        cout << "Receive" << endl;
    }
};