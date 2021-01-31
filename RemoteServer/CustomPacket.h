#pragma once
#include "Packet.h"

class CustomPacket : public Packet
{
public:
    enum { UPLOAD = 1, DOWNLOAD = 2, END = 3 };
    void SetCommand(int command);
    void SetSize(int size);
    void SetPrefix(const char* prefix);
    void SetData(std::vector<char> data);
    const int GetCommand() const;
    const int GetSize() const;
    const char* GetPrefix() const;
    const std::vector<char> GetData() const;
    virtual const bool OnParse(const char* buffer, const unsigned int bufferSize) override;
    virtual const bool OnBuild(char* buffer, unsigned int& buildBufferSize) override;
    virtual const void Clear() override;
    virtual const int GetHeaderSize() override;

private:
    char prefix[8];
    int command;
    int size;
    std::vector<char> data;
};