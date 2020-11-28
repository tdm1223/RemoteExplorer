#include "Packet.h"

void MyPacket::SetCommand(int command)
{
    this->command = command;
}

void MyPacket::SetSize(int size)
{
    this->size = size;
}

void MyPacket::SetPrefix(const char* prefix)
{
    strcpy(this->prefix, prefix);
}

void MyPacket::SetData(std::vector<char> data)
{
    this->data = data;
}

const int MyPacket::GetCommand() const
{
    return command;
}

const int MyPacket::GetSize() const
{
    return size;
}

const char* MyPacket::GetPrefix() const
{
    return prefix;
}

const std::vector<char> MyPacket::GetData() const
{
    return data;
}

const bool MyPacket::OnParse(const char* buffer, const unsigned int bufferSize)
{
    if (buffer == nullptr || bufferSize < GetHeaderSize() + GetSize())
    {
        return false;
    }

    int offset = 0;
    memcpy(&prefix[0], buffer, 8);
    offset += 8;

    if (strcmp(prefix, kPrefix.c_str()) != 0)
    {
        return false;
    }

    memcpy(&command, buffer + offset, 4);
    offset += 4;

    memcpy(&size, buffer + offset, 4);
    offset += 4;

    data.resize(size);
    memcpy(&data[0], buffer + offset, size);
    offset += size;

    if (offset != bufferSize)
    {
        return false;
    }

    return true;
}

const bool MyPacket::OnBuild(char* buffer, unsigned int& buildBufferSize)
{
    if ((command > END || command < UPLOAD) || size < 0)
    {
        return false;
    }

    memcpy(buffer, &prefix, sizeof(char) * 8);
    buildBufferSize += 8;

    memcpy(buffer + buildBufferSize, &command, 4);
    buildBufferSize += 4;

    memcpy(buffer + buildBufferSize, &size, 4);
    buildBufferSize += 4;

    memcpy(buffer + buildBufferSize, &data[0], size);
    buildBufferSize += size;

    return true;
}

const void MyPacket::Clear()
{
    memset(prefix, 0, 8);
    command = -1;
    size = -1;
    data.clear();
}

const int MyPacket::GetHeaderSize()
{
    // prefix + command + size
    return 2 * sizeof(int) + 8 * sizeof(char);
}
