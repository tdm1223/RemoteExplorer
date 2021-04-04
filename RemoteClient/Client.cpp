#include"Client.h"
#include<thread>

Client::Client()
{

}

#include<iostream>
void Client::Initialize()
{
    clientSock = ClientSocket::GetInstance();
    if (clientSock->Connect(kPort))
    {
        clientSock->Loop();
    }
    // 소켓을 닫음
    clientSock->CloseSocket();
}