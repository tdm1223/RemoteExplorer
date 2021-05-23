#include "EchoCommand.h"

bool EchoCommand::Execute(SOCKET sock, void* buffer, int bufferLength)
{
    // COMMAND SEND
    char echoData[Util::kBufferSize];
    memset(echoData, 0, Util::kBufferSize);
    memcpy(echoData, (char*)buffer, bufferLength);
    std::cout << "echo send data : " << echoData << std::endl;

    char buffers[Util::kBufferSize];
    memset(buffers, 0, Util::kBufferSize);
    if (!SendCommandWithData(sock, buffers, Util::COMMAND::ECHO, echoData))
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
