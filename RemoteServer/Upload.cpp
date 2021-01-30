#include "Upload.h"

void Upload::Execute(SOCKET& sock, std::string& name)
{
    char recvBuffer[4096];
    std::cout << "Upload start" << std::endl;

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
    std::cout << "파일이름 : " << name << " 파일 크기 : " << fileSize << std::endl;

    // 파일 받는 로직
    FILE* fp = fopen(name.c_str(), "wb+");
    std::cout << "파일명 : " << name << std::endl;

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
        //std::cout << "받은 크기 : " << filePacket.GetHeaderSize() << " 전체 크기 : " << totalSize << std::endl;
        fwrite(&packet.GetData()[0], 1, recvLength - packet.GetHeaderSize(), fp);
    }

    fclose(fp);
    std::cout << "전송 완료" << std::endl;
}
