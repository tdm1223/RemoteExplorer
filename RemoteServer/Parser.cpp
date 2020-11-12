#include "parser.h"

bool Parser::Parsing(char* recvBuffer, int byteLen, Packet& resultPacket)
{
    // header + body
    //------------------------
    if (byteLen < packetParsingSize)
    {
        return false;
    }

    int offset = 0;
    memcpy(&resultPacket.command, recvBuffer[0], 4);
    offset += 4;

    memcpy(&resultPacket.size, recvBuffer[offset], 4);
    offset += 4;
   
    for (int i = 0; i < byteLen; i++)
    {
        const auto b = recvBuffer[i];

        switch (cnt_)
        {
        case 0:
            if (b != 0x7F)
            {
                cnt_ = 0;
                continue;
            }
            resultPacket.prefix = 0x7F;
            cnt_++;
            break;
        case 1:
            resultPacket.command = int(b) << 24;
            cnt_++;
            break;
        case 2:
            resultPacket.command |= int(b) << 16;
            cnt_++;
            break;
        case 3:
            resultPacket.command |= int(b) << 8;
            cnt_++;
            break;
        case 4:
            resultPacket.command |= int(b) << 0;
            resultPacket.command = ntohl(resultPacket.command);
            cnt_++;
            break;
        case 5:
            resultPacket.size = int(b) << 24;
            cnt_++;
            break;
        case 6:
            resultPacket.size |= int(b) << 16;
            cnt_++;
            break;
        case 7:
            resultPacket.size |= int(b) << 8;
            cnt_++;
            break;
        case 8:
            resultPacket.size |= int(b) << 0;
            resultPacket.size = ntohl(resultPacket.size);
            cnt_++;
            resultPacket.buf.clear();
            if (resultPacket.size > 0)
            {
                resultPacket.buf.reserve(resultPacket.size);
            }
            else
            {
                cnt_++;
            }
            break;
        case 9:
            resultPacket.buf.push_back(b);
            if (resultPacket.buf.size() == resultPacket.size)
            {
                cnt_ = 0;
                return true;
            }
            break;
        }
    }
    return false;
}