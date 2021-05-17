#include"UploadCommand.h"

bool UploadCommand::Execute(SOCKET sock, char* buf)
{
    std::cout << "Upload" << std::endl;
    return true;
}