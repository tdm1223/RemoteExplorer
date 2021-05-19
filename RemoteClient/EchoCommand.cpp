#include "EchoCommand.h"

bool EchoCommand::Execute(SOCKET sock, void* buffer)
{
    // COMMAND SEND
    std::string echoData;
    echoData = (char*)buffer;

    char buffers[Util::kBufferSize];
    memset(buffers, 0, Util::kBufferSize);
    if (!SendCommandWithData(sock, buffers, Util::COMMAND::UPLOAD, echoData))
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
