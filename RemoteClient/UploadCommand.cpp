#include "UploadCommand.h"

bool UploadCommand::Execute(SOCKET sock, void* buffer)
{
    std::cout << "UploadCommand" << std::endl;
    std::string message;
    std::cout << "입력 : ";
    std::cin >> message;

    memset((char*)buffer, 0, Util::kBufferSize);
    // COMMAND SEND
    if (!SendCommand(sock, (char*)buffer, Util::COMMAND::UPLOAD))
    {
        return false;
    }

    return false;
}
