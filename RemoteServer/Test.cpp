#include "Test.h"

void Test::Execute(SOCKET& sock)
{
    char recvBuffer[4096];

    packet.Clear();
    int recvLength = 0;
    while (1)
    {
        recvLength = recv(sock, recvBuffer, bufSize, 0);
        if (recvLength == -1)
        {
            continue;
        }
        break;
    }

    packet.Parsing(recvBuffer, recvLength);
    std::string size;
    for (int i = 0; i < packet.GetSize(); i++)
    {
        size.push_back(packet.GetData()[i]);
    }
    int fileSize = atoi(size.c_str());

    memset(recvBuffer, 0, sizeof(recvBuffer));
    recvLength = 0;
    int totalSize = 0;

    while ((recvLength = recv(sock, recvBuffer, bufSize, 0)) != 0)
    {
        packet.Clear();
        packet.Parsing(recvBuffer, recvLength);
        if (GetLastError() == WSAEWOULDBLOCK)
        {
            Sleep(50); // 잠시 기다렸다가 재전송
            if (totalSize == fileSize)
            {
                std::cout << "파일 받기 완료" << std::endl;
                break;
            }
            continue;
        }
        totalSize += (recvLength - packet.GetHeaderSize());
    }

    std::cout << "전송 완료" << std::endl;
}
