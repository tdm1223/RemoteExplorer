#include "DownloadCommand.h"

bool DownloadCommand::Execute(SOCKET s, void* buffer)
{
    std::cout << "DownloadCommand" << std::endl;
    char buf[4096];
    memset(buf, 0, 4096);

    command = 2;
    int buildBufferSize = 0;

    memcpy(buf + buildBufferSize, prefix.c_str(), sizeof(char) * 8);
    buildBufferSize += 8;

    memcpy(buf + buildBufferSize, &command, 4);
    buildBufferSize += 4;

    std::cout << buildBufferSize << std::endl;
    if (send(s, buf, buildBufferSize, false))
    {
        std::cout << "send success" << std::endl;
    }

    return true;
}
