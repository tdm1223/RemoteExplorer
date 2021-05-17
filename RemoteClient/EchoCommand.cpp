#include "EchoCommand.h"

bool EchoCommand::Execute(SOCKET sock, void* buffer)
{
    // COMMAND SEND
    std::string fileName;
    std::cout << "입력 : ";
    std::cin >> fileName;

    char buffers[Util::kBufferSize];
    memset(buffers, 0, Util::kBufferSize);
    if (!SendCommandWithData(sock, buffers, Util::COMMAND::UPLOAD, fileName))
    {
        return false;
    }

    int size = 0;
    char recvBuf[Util::kMaxFileNameLength];
    memset(recvBuf, 0, Util::kMaxFileNameLength);
    if (Recv(sock, recvBuf, &size))
    {
        std::cout << "ECHO : " << recvBuf << std::endl;
    }
    return false;
}
