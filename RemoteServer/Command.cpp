#include"Command.h"
#include"UploadCommand.h"
#include"DownloadCommand.h"
#include"ListCommand.h"

Command* GetCommand(const std::string& command)
{
    if (command == "UPLOAD")
    {
        return (Command*)new UploadCommand;
    }
    else if (command == "DOWNLOAD")
    {
        return (Command*)new DownloadCommand;
    }
    else if (command == "LIST")
    {
        return (Command*)new ListCommand;
    }
    else
    {
        OutputDebugString(L"Unknown command");
        return NULL;
    }
}

void Command::SerializeInt(const int input, char* output)
{
    SecureZeroMemory(output, 4);
    int* intPointer = reinterpret_cast<int*>(output);
    *intPointer = input;
}