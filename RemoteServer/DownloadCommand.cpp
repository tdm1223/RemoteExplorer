#include"DownloadCommand.h"
#include"Util.h"

bool DownloadCommand::Execute(SOCKET sock, char* buf)
{
    std::cout << "Download" << std::endl;

    // RECV
    std::cout << "BUF : " << buf << std::endl;
    return true;
}