#include"Command.h"
#include"Upload.h"
#include"Download.h"
#include"Move.h"

Command* GetCommand(const std::string& command)
{
    if (command == "UPLOAD")
    {
        return (Command*)new Upload;
    }
    else if (command == "DOWNLOAD")
    {
        return (Command*)new Download;
    }
    else if (command == "MOVE")
    {
        return (Command*)new Move;
    }
    else
    {
        OutputDebugString(L"Unknown command");
        return NULL;
    }
}