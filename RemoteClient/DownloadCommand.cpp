#include "DownloadCommand.h"

bool DownloadCommand::Execute(SOCKET sock, void* buffer)
{
    std::cout << "DoanloadCommand" << std::endl;
    std::string message;
    std::cout << "ют╥б : ";
    std::cin >> message;

    memset((char*)buffer, 0, 4096);

    char* msg = (char*)buffer;

    command = 2;
    int buildBufferSize = 0;

    memcpy(msg + buildBufferSize, prefix.c_str(), sizeof(char) * 8);
    buildBufferSize += 8;

    memcpy(msg + buildBufferSize, &command, 4);
    buildBufferSize += sizeof(int);

    int length = message.length();
    memcpy(msg + buildBufferSize, &length, sizeof(int));
    buildBufferSize += sizeof(int);
    std::cout << length << std::endl;

    memcpy(msg + buildBufferSize, message.c_str(), message.length());
    buildBufferSize += message.length();

    std::cout << buildBufferSize << std::endl;
    if (send(sock, msg, buildBufferSize, false))
    {
        std::cout << "send success" << std::endl;
    }

    return false;
}
