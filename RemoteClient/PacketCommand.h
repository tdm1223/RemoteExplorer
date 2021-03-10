#ifndef PACKETCOMMAND_H
#define PACKETCOMMAND_H

#include<string>
#include<vector>
#include <WinSock2.h>

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

    virtual bool Execute(SOCKET s, void* buffer) = 0;

    std::string prefix = "ESTSOFT";
    int command = 0;
    int size = 0;
    std::vector<char> data;
    const int kBufSize = 4096;

};

#endif
