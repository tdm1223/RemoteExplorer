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
    return true;
}
