#include "DownloadCommand.h"

bool DownloadCommand::Execute(SOCKET s, void* buffer)
{
    std::cout << "DownloadCommand" << std::endl;
    return true;
}
