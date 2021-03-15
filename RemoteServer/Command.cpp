#include"Command.h"
#include"Upload.h"

Command* GetCommand(const std::string& command)
{
    if (command == "UPLOAD")
    {
        return (Command*)new Upload;
    }
    else if (command == "DOWNLOAD")
    {
    }
    else
    {
        OutputDebugString(L"Unknown command");
        return NULL;
    }
}