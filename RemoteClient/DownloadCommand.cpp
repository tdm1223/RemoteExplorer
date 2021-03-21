#include "DownloadCommand.h"

bool DownloadCommand::Execute(SOCKET sock, void* buffer)
{
    std::cout << "DoanloadCommand" << std::endl;
    std::string message;
    std::cout << "ют╥б : ";
    std::cin >> message;

    memset((char*)buffer, 0, Util::kBufferSize);
    // COMMAND SEND
    if (!SendCommand(sock, (char*)buffer, Util::COMMAND::DOWNLOAD))
    {
        return false;
    }

    return false;
}
