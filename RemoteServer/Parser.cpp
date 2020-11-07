#include "parser.h"

bool Parser::Parsing(char* recvBuffer, int byteLen, Packet& resultPacket)
{
    for (int i = 0; i < byteLen; i++)
    {
        const auto b = recvBuffer[i];

        switch (step)
        {
        case 0:
            if (b != 0x7F)
            {
                step = 0;
                continue;
            }
            resultPacket.prefix = 0x7F;
            step++;
            break;
        case 1:
            resultPacket.command = int(b) << 24;
            step++;
            break;
        case 2:
            resultPacket.command |= int(b) << 16;
            step++;
            break;
        case 3:
            resultPacket.command |= int(b) << 8;
            step++;
            break;
        case 4:
            resultPacket.command |= int(b) << 0;
            resultPacket.command = ntohl(resultPacket.command);
            step++;
            break;
        case 5:
            resultPacket.size = int(b) << 24;
            step++;
            break;
        case 6:
            resultPacket.size |= int(b) << 16;
            step++;
            break;
        case 7:
            resultPacket.size |= int(b) << 8;
            step++;
            break;
        case 8:
            resultPacket.size |= int(b) << 0;
            resultPacket.size = ntohl(resultPacket.size);
            step++;
            resultPacket.buf.clear();
            if (resultPacket.size > 0)
            {
                resultPacket.buf.reserve(resultPacket.size);
            }
            else
            {
                step++;
            }
            break;
        case 9:
            resultPacket.buf.push_back(b);
            if (resultPacket.buf.size() == resultPacket.size)
            {
                step = 0;
                return true;
            }
            break;
        }
    }
    return false;
}