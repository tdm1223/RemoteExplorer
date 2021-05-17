#include "PacketCommand.h"
#include<iostream>

PacketCommand::PacketCommand()
{
}

PacketCommand::~PacketCommand()
{

}

void PacketCommand::SerializeInt(const int input, char* output)
{
    memset(output, 0, 4);
    int* ptr = reinterpret_cast<int*>(output);
    *ptr = input;
}

bool PacketCommand::SendCommandWithData(SOCKET& sock, char* message, int command, std::string data)
{
    int buildBufferSize = 0;

    memcpy(message + buildBufferSize, prefix.c_str(), Util::kPrefixSize);
    buildBufferSize += Util::kPrefixSize;

    memcpy(message + buildBufferSize, &command, Util::kCommandSize);
    buildBufferSize += Util::kCommandSize;

    int length = data.size();
    memcpy(message + buildBufferSize, &length, Util::kLengthSize);
    buildBufferSize += Util::kLengthSize;

    memcpy(message + buildBufferSize, data.c_str(), data.size());
    buildBufferSize += data.size();

    if (send(sock, message, buildBufferSize, false) == SOCKET_ERROR)
    {
        return false;
    }
    return true;
}

bool PacketCommand::Send(SOCKET& sock, const char* message)
{
    // 메세지 4바이트에 길이를 담아 보냄
    char buffer[Util::kLengthSize] = { 0 };
    int length = 0;
    SerializeInt(length, buffer);

    if (send(sock, buffer, Util::kLengthSize, 0) == SOCKET_ERROR)
    {
        return false;
    }

    if (send(sock, message, static_cast<int32_t>(strlen(message)), 0) == SOCKET_ERROR)
    {
        return false;
    }

    return true;
}

bool PacketCommand::SendCommand(SOCKET& sock, char* message, int command)
{
    int buildBufferSize = 0;

    memcpy(message + buildBufferSize, prefix.c_str(), Util::kPrefixSize);
    buildBufferSize += Util::kPrefixSize;

    memcpy(message + buildBufferSize, &command, Util::kCommandSize);
    buildBufferSize += Util::kCommandSize;

    if (send(sock, message, buildBufferSize, false) == SOCKET_ERROR)
    {
        return false;
    }
    return true;
}

int PacketCommand::DeserializeInt(const char* input)
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

bool PacketCommand::RecvLength(SOCKET& sock, int* output)
{
    char buffer[Util::kLengthSize];
    if (recv(sock, buffer, Util::kLengthSize, false) == SOCKET_ERROR)
    {
        return false;
    }

    // 메세지의 첫번째 Byte가 길이
    *output = DeserializeInt(buffer);
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
    char buffer[Util::kBufferSize];
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