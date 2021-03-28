#include"Command.h"
#include"UploadCommand.h"
#include"DownloadCommand.h"
#include"ListCommand.h"
#include"Util.h"

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
    memset(output, 0, Util::kLengthSize);
    int* ptr = reinterpret_cast<int*>(output);
    *ptr = input;
}

bool Command::SendLength(SOCKET& sock, int length)
{
    char buffer[Util::kLengthSize] = { 0 };
    SerializeInt(length, buffer);

    if (send(sock, buffer, Util::kLengthSize, 0) == SOCKET_ERROR)
    {
        return false;
    }

    return true;
}

bool Command::Send(SOCKET& sock, char* message)
{
    // 길이 전송
    if (!SendLength(sock, static_cast<int32_t>(strlen(message))))
    {
        return false;
    }

    // 길이 만큼 데이터 전송
    if (send(sock, message, static_cast<int32_t>(strlen(message)), 0) == SOCKET_ERROR)
    {
        return false;
    }
    return true;
}