#pragma once
#include<WinSock2.h>
#include"ClientSocket.h"

#pragma comment(lib, "ws2_32.lib")

class Client
{
public:
    Client();
    bool Initialize();
    void Loop();
    void Close();
    char sendBuffer[Util::kBufferSize];

private:
    const int kPort = 9000;
    ClientSocket* clientSocket;
};