#include "MoveCommand.h"

bool MoveCommand::Execute(SOCKET s, void* buffer)
{
    int buildBufferSize = 0;
    std::cout << "MoveCommand" << std::endl;

    memcpy(&buffer + buildBufferSize, &prefix, 8);
    buildBufferSize += 4;

    memcpy(&buffer + buildBufferSize, &command, 4);
    buildBufferSize += 4;

    memcpy(&buffer + buildBufferSize, &size, 4);
    buildBufferSize += 4;

    std::cout << buildBufferSize << std::endl;
    if (send(s, (char*)buffer, buildBufferSize, false))
    {
        std::cout << "send success" << std::endl;
    }
    return true;
}
