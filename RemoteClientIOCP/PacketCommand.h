﻿#ifndef PACKETCOMMAND_H
#define PACKETCOMMAND_H

#include<string>
#include<vector>
#include <WinSock2.h>
#include"Util.h"
#include<iostream>

class PacketCommand
{
public:
    PacketCommand();
    virtual ~PacketCommand();
    virtual bool Execute(SOCKET s, void* buffer, int length) = 0;

    std::string prefix = "ESTSOFT";
    int command = 0;
    int size = 0;
    std::vector<char> data;

    void SerializeInt(const int input, char* output);
    bool SendCommand(SOCKET& sock, char* message, int command);
    bool SendCommandWithData(SOCKET& sock, char* message, int command, std::string data);

    int DeserializeInt(const char* input);
    bool RecvLength(SOCKET& sock, int* outputInt);
    bool Recv(SOCKET& sock, char* outputString, int* size);

    bool Send(SOCKET& sock, const char* message);
private:
};

#endif
