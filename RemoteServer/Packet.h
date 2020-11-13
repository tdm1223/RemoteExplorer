#pragma once
#include<vector>
#include<winsock.h>
#include<iostream>

class Packet
{
public:
    virtual const bool OnParse(const char* buffer, const unsigned int bufferSize) = 0;
    virtual const bool OnBuild(char* buffer, unsigned int& buildBufferSize) = 0;
    virtual const void Clear() = 0;

    const std::string kPrefix = "ESTSOFT";
};

class MyPacket : public Packet
{
public:
    enum { UPLOAD = 1, DOWNLOAD = 2, END = 3 };
    void SetCommand(int command)
    {
        this->command = command;
    }

    void SetSize(int size)
    {
        this->size = size;
    }

    void SetPrefix(const char* prefix)
    {
        strcpy(this->prefix, prefix);
    }

    void SetData(std::vector<char> data)
    {
        this->data = data;
    }

    const int GetCommand() const
    {
        return command;
    }

    const int GetSize() const
    {
        return size;
    }

    const char* GetPrefix() const
    {
        return prefix;
    }

    const std::vector<char> GetData() const
    {
        return data;
    }

    virtual const bool OnParse(const char* buffer, const unsigned int bufferSize) override
    {
        if (buffer == nullptr) //  || bufferSize < header + body
        {
            return false;
        }

        int offset = 0;

        memcpy(&prefix[0], buffer, 8);
        offset += 8;

        if (strcmp(prefix, kPrefix.c_str()) != 0)
        {
            std::cout << "´Ù¸£´Ù..." << std::endl;
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

    virtual const bool OnBuild(char* buffer, unsigned int& buildBufferSize) override
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

    virtual const void Clear() override
    {
        memset(prefix, 0, 8);
        command = -1;
        size = -1;
        data.clear();
    }

private:
    char prefix[8];
    int command;
    int size;
    std::vector<char> data;
};

//server code
//{
//    //case Send
//    MyPacket packet;
//   packet.command = 4;
//    packet.size = 8;
//
//    char pakcetBuffer[MAX_PACKET] = { 0, };
//    uint32_t buildBufferSize = 0;
//    if (packet.OnBuild(packetBuffer, buildBufferSize) == true)
//    {
//        Send(socket, packetBuffer, buildBufferSize);
//    }
//
//    //case Recv
//    MyPacket packet;
//    if (packet.OnParse(recvBuffer, 8) == true)
//    {
//        packet.GetCommand();
//    }
//
//}
//
//class Packet
//{
//public:
//    Packet();
//    ~Packet();
//
//    void WriteWithoutData(int command, int size, int& offset);
//    void WriteData(char* data, int size, int& offset);
//    void Serialize(char* data);
//    void Build(char* buffer, int command, int size, char* data, int& offset);
//    int GetHeaderSize();
//    char prefix = 0x7F;
//    int command;
//    int size;
//
//    std::vector<char> buf;
//};