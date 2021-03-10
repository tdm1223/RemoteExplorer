#include"Client.h"
#include<thread>

Client::Client()
{

}

#include<iostream>
void Client::Initialize()
{
    clientSock = ClientSocket::GetInstance();
    clientSock->Connect(kPort);

    // 소켓을 닫음
    clientSock->CloseSocket();
}