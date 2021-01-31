#include "Command.h"
#include "Upload.h"
#include "Download.h"

Command* GetCommand(const std::string& command)
{
    if (command == "UPLOAD")
    {
        return (Command*)new Upload;
    }
    else if (command == "DOWNLOAD")
    {
        return (Command*) new Download;
    }
}