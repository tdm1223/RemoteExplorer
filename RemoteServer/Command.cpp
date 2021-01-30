#include "Command.h"
#include "Upload.h"
#include "Download.h"

Command* GetCommand(const std::string& command)
{
    if (command == "UPLOAD")
    {
        std::cout << "UPLOAD 생성 완료" << std::endl;
        return (Command*)new Upload;
    }
    else if (command == "DOWNLOAD")
    {
        std::cout << "DOWNLOAD 생성 완료" << std::endl;
        return (Command*) new Download;
    }
}