#include "ListCommand.h"

bool ListCommand::Execute(SOCKET sock, void* buffer)
{
    std::cout << "ListCommand" << std::endl;

    memset((char*)buffer, 0, 4096);

    char* msg = (char*)buffer;

    command = 3;
    int buildBufferSize = 0;

    memcpy(msg + buildBufferSize, prefix.c_str(), sizeof(char) * 8);
    buildBufferSize += 8;

    memcpy(msg + buildBufferSize, &command, 4);
    buildBufferSize += sizeof(int);

    int length = 0;
    memcpy(msg + buildBufferSize, &length, sizeof(int));
    buildBufferSize += sizeof(int);
    std::cout << length << std::endl;

    std::cout << buildBufferSize << std::endl;
    if (send(sock, msg, buildBufferSize, false))
    {
        std::cout << "send success" << std::endl;
    }

    // recv
    int size = 0;
    char buf[kBufferSize];
    memset(buf, 0, kBufferSize);
    Recv(sock, buf, &size);

    for (int i = 0; i < size; i++)
    {
        std::cout << buf[i];
    }
    std::cout << std::endl;
    return false;
}
