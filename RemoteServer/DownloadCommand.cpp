#include"DownloadCommand.h"
#include"Util.h"

bool DownloadCommand::Execute(SOCKET sock, char* buf)
{
    std::cout << "Download" << std::endl;

    int cnt = 0;

    // RECV
    // 파일 전송
    int fileSize = 0;

    FILE* fp = fopen(buf, "rb");
    if (fp == NULL)
    {
        std::cout << "없는 파일" << std::endl;
        return false;
    }
    std::cout << "있는 파일" << std::endl;
    fseek(fp, 0, SEEK_END);
    fileSize = ftell(fp);

    char buffer[Util::kLengthSize];
    SerializeInt(fileSize, buffer);
    send(sock, buffer, Util::kLengthSize, 0);
    rewind(fp);
    int sendSize = 0;
    // 파일 크기가 0보다 크다면 파일 전송
    if (fileSize > 0)
    {
        while (fileSize > 0)
        {
            char dataBuffer[Util::kBufferSize];
            memset(dataBuffer, 0, Util::kBufferSize);
            sendSize = fread(dataBuffer, 1, fileSize, fp);
            std::cout << "sendsize  : " << sendSize << std::endl;
            SerializeInt(sendSize, buffer);
            send(sock, buffer, Util::kLengthSize, 0);
            if (sendSize < Util::kBufferSize)
            {
                std::cout << sendSize << "보냄" << std::endl;
                send(sock, dataBuffer, sendSize, 0);
                break;
            }
            send(sock, dataBuffer, Util::kBufferSize, 0);
            fileSize -= sendSize;
        }
    }

    fclose(fp);
    // 0이면 파일이 없는것
    return true;
}