#pragma once
#include "packet.h"
#include<iostream>

class Parser
{
public:
    /*
        \param data
            The buffer of data which the function parses.
        \param len
            The len of `data` buffer.
        \param[in,out] bytesRead
            Will contain the number of bytes read by `parse()`.
            Note: It may be possible that the function returns
            before all bytes of `data` have been parsed, because
            the `packet` is complete.
        \param[in,out] packet
            Instance of `Packet` which will be filled from `data` by this  function.
    */
    bool parse(char* data, int len, int& bytesRead, Packet& packet);

private:
    int _step = 0;
};
