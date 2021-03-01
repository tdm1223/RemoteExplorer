#include"Client.h"
#include<thread>

Client::Client()
{
    clientSock = ClientSocket::GetInstance();
    clientSock->Connect(kPort);
}

#include<iostream>
void Client::Initialize()
{
    // 소켓을 닫음
    clientSock->CloseSocket();
}