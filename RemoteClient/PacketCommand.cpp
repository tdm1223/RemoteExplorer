#include "PacketCommand.h"

PacketCommand::PacketCommand()
{
}

PacketCommand::~PacketCommand()
{

}

void PacketCommand::SerializeInt(const int input, char* output)
{
    SecureZeroMemory(output, 4);
    int* intPointer = reinterpret_cast<int*>(output);
    *intPointer = input;
}

bool PacketCommand::SendLength(SOCKET& sock, int length)
{
    // 길이를 담아서 보냄
    char buffer[kBufferSize] = { 0 };
    SerializeInt(length, buffer);

    if (send(sock, buffer, kBufferSize, 0) == SOCKET_ERROR)
    {
        return false;
    }

    return true;
}

bool PacketCommand::Send(SOCKET& sock, char* message)
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
}

int PacketCommand::DeserializeInt(const char* input)
{
    int output = 0;

    char* charPointer = reinterpret_cast<char*>(&output);
    for (int i = 0; i < 3; i++)
    {
        *charPointer = input[i];
        ++charPointer;
    }
    *charPointer = input[3];
    return output;
}

bool PacketCommand::RecvLength(SOCKET& sock, int* outputInt)
{
    char buffer[kLengthMessageSize];

    if (recv(sock, buffer, kLengthMessageSize, false) == SOCKET_ERROR)
    {
        return false;
    }

    // 메세지의 첫번째 Byte가 길이
    *outputInt = DeserializeInt(buffer);

    return true;
}

bool PacketCommand::Recv(SOCKET& sock, char* outputString, int* size)
{
    // 길이 받음
    int length;
    if (!RecvLength(sock, &length))
    {
        return false;
    }

    if (length == 0)
    {
        return false;
    }

    // 길이 만큼 데이터 받음
    char buffer[kBufferSize];
    int receivedBytes = 0;
    int totalBytes = length;
    char* bufferPointer = buffer;
    do
    {
        receivedBytes = recv(sock, bufferPointer, totalBytes, false);
        if (receivedBytes == SOCKET_ERROR)
        {
            return false;
        }
        totalBytes -= receivedBytes;
        bufferPointer += receivedBytes;
    } while (totalBytes > 0);

    memcpy_s(outputString, length, buffer, length);
    outputString[length] = '\0';

    if (size != nullptr)
    {
        *size = length;
    }

    return true;
}