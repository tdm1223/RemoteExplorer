#ifndef PACKETCOMMAND_H
#define PACKETCOMMAND_H

#include<string>
#include<vector>

enum protocolType
{
    kUpload,
    kDownload,
    kConnect
};

class PacketCommand
{
public:
    PacketCommand();
    virtual ~PacketCommand();
    virtual bool Execute() = 0;
    const bool OnParse(const char* buffer, const unsigned int bufferSize);
    const bool OnBuild(char* buffer, unsigned int& buildBufferSize);
    const void Clear();
    const unsigned int GetHeaderSize();
    void SetCommand(int command);
    void SetSize(int size);
    void SetData(std::vector<char> data);

    const int GetCommand() const;
    const int GetSize() const;
    const std::string GetPrefix() const;
    const std::vector<char> GetData() const;

private:
    std::string prefix = "ESTSOFT";
    int command = 0;
    int size = 0;
    std::vector<char> data;
};

#endif
