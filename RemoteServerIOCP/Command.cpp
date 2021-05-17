#include"Command.h"
#include"EchoCommand.h"
#include"DownloadCommand.h"
#include"ListCommand.h"
#include"Util.h"

Command* GetCommand(const std::string& command)
{
    if (command == "ECHO")
    {
        return (Command*)new EchoCommand;
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
    wsaBuf.buf = buffer;
    wsaBuf.len = Util::kLengthSize;

    if (WSASend(sock, &wsaBuf, 1, (LPDWORD)&sendBytes, 0, NULL, NULL) == SOCKET_ERROR)
    {
        return false;
    }

    //if (send(sock, buffer, Util::kLengthSize, 0) == SOCKET_ERROR)
    //{
    //    return false;
    //}

    return true;
}

bool Command::Send(SOCKET& sock, const char* message)
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

int Command::DeserializeInt(const char* input)
{
    int output = 0;

    char* ptr = reinterpret_cast<char*>(&output);
    for (int i = 0; i < 3; i++)
    {
        *ptr = input[i];
        ++ptr;
    }
    *ptr = input[3];
    return output;
}

bool Command::RecvLength(SOCKET& sock, int* output)
{
    char buffer[Util::kLengthSize] = { 0 };

    if (recv(sock, buffer, Util::kLengthSize, false) == SOCKET_ERROR)
    {
        return false;
    }

    // 메세지의 첫번째 Byte가 길이
    *output = DeserializeInt(buffer);
    std::cout << "LENGTH : " << output << std::endl;
    return true;
}

bool Command::Recv(SOCKET& sock, char* outputString, int* size)
{
    // 길이 받음
    int length = 0;
    if (!RecvLength(sock, &length))
    {
        return false;
    }

    if (length == 0)
    {
        return false;
    }

    // 길이 만큼 데이터 받음
    char buffer[Util::kBufferSize];
    int receivedBytes = 0;
    int totalBytes = length;
    char* bufferPointer = buffer;
    receivedBytes = recv(sock, bufferPointer, totalBytes, false);
    memcpy_s(outputString, length, buffer, length);
    outputString[length] = '\0';

    if (size != nullptr)
    {
        *size = length;
    }

    return true;
}