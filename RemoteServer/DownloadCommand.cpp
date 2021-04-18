#include"DownloadCommand.h"
#include"Util.h"

bool DownloadCommand::Execute(SOCKET sock, char* buf)
{
    // SERVER -> Client 파일 전송
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

    char lengthSizeBuffer[Util::kLengthSize];
    memset(lengthSizeBuffer, 0, Util::kLengthSize);

    SerializeInt(fileSize, lengthSizeBuffer);
    if (send(sock, lengthSizeBuffer, Util::kLengthSize, 0) == SOCKET_ERROR)
    {
        fclose(fp);
        return false;
    }

    rewind(fp);
    int sendSize = 0;

    // 파일 크기가 0보다 크다면 파일 전송
    char dataBuffer[Util::kBufferSize];
    while (fileSize > 0)
    {
        memset(dataBuffer, 0, Util::kBufferSize);

        sendSize = fread(dataBuffer, 1, Util::kBufferSize, fp);
        SerializeInt(sendSize, lengthSizeBuffer);
        if (send(sock, lengthSizeBuffer, Util::kLengthSize, 0) == SOCKET_ERROR)
        {
            fclose(fp);
            return false;
        }

        if (sendSize < Util::kBufferSize)
        {
            if (send(sock, dataBuffer, sendSize, 0) == SOCKET_ERROR)
            {
                fclose(fp);
                return false;
            }
            break;
        }

        if (send(sock, dataBuffer, Util::kBufferSize, 0) == SOCKET_ERROR)
        {
            fclose(fp);
            return false;
        }
        fileSize -= sendSize;
    }

    fclose(fp);
    return true;
}