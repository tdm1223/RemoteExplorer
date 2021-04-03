#include "DownloadCommand.h"

bool DownloadCommand::Execute(SOCKET sock, void* buffer)
{
    // COMMAND SEND
    char buffers[Util::kBufferSize];
    memset(buffers, 0, Util::kBufferSize);
    std::cout << "DownloadCommand" << std::endl;
    std::string fileName;
    std::cout << "입력 : ";
    std::cin >> fileName;

    memset(buffers, 0, Util::kBufferSize);
    if (!SendCommandWithData(sock, buffers, Util::COMMAND::DOWNLOAD, fileName))
    {
        return false;
    }
    std::cout << "CLIENT SUCCESS" << std::endl;

    int length = 0;
    RecvLength(sock, &length);
    std::cout << "LENGTH : " << length << std::endl;

    FILE* fp;
    if (length > 0)
    {
        while (1)
        {
            fp = fopen(fileName.c_str(), "wb+");
            RecvLength(sock, &length);
            std::cout << "length : " << length << std::endl;
            memset(buffers, 0, Util::kBufferSize);
            if (length < Util::kBufferSize)
            {
                recv(sock, buffers, length, 0);
                fwrite(buffers, 1, length, fp);
                break;
            }
            recv(sock, buffers, length, 0);
            fwrite(buffers, 1, length, fp);
        }
        fclose(fp);
    }
    return true;
}
