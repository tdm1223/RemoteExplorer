#include "DownloadCommand.h"

bool DownloadCommand::Execute(SOCKET sock, void* buffer)
{
    // COMMAND SEND
    std::string fileName;
    std::cout << "입력 : ";
    std::cin >> fileName;

    char buffers[Util::kBufferSize];
    memset(buffers, 0, Util::kBufferSize);
    if (!SendCommandWithData(sock, buffers, Util::COMMAND::DOWNLOAD, fileName))
    {
        return false;
    }

    int length = 0;
    if (!RecvLength(sock, &length))
    {
        return false;
    }
    std::cout << "FILE SIZE : " << length << std::endl;

    if (length > 0)
    {
        FILE* fp;
        fp = fopen(fileName.c_str(), "wb+");
        while (1)
        {
            if (!RecvLength(sock, &length))
            {
                fclose(fp);
                return false;
            }

            std::cout << "length : " << length << std::endl;
            memset(buffers, 0, Util::kBufferSize);

            if (length < Util::kBufferSize)
            {
                if (recv(sock, buffers, length, 0) < 0)
                {
                    fclose(fp);
                    return false;
                }
                fwrite(buffers, 1, length, fp);
                break;
            }

            if (recv(sock, buffers, length, 0) < 0)
            {
                fclose(fp);
                return false;
            }
            fwrite(buffers, 1, length, fp);
        }
        fclose(fp);
    }
    return true;
}
