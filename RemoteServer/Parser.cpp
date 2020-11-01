//#include "parser.h"
//
//bool Parser::Parse(char* data, int len, int& bytesRead, Packet& packet)
//{
//    bytesRead = 0;
//    for (int i = 0; i < len; ++i)
//    {
//        const auto b = data[i];
//        bytesRead++;
//        switch (_step)
//        {
//            // prefix
//        case 0:
//            if (b != 0x7F)
//            {
//                _step = 0;
//                continue;
//            }
//            packet.prefix = b;
//            _step++;
//            break;
//
//            // command
//        case 1:
//            packet.command = char(b);
//            _step++;
//            break;
//
//            // Size
//        case 2:
//            packet.size = int(b) << 24;
//            _step++;
//            break;
//
//        case 3:
//            packet.size |= int(b) << 16;
//            _step++;
//            break;
//
//        case 4:
//            packet.size |= int(b) << 8;
//            _step++;
//            break;
//
//        case 5:
//            packet.size |= int(b) << 0;
//            packet.size = ntohl(packet.size);
//            _step++;
//
//            packet.data.clear();
//            if (packet.size > 0)
//            {
//                packet.data.reserve(packet.size);
//            }
//            else
//            {
//                _step++;
//            }
//            break;
//
//            // Data
//        case 6:
//            packet.data.push_back(b);
//
//            if (packet.data.size() == packet.size)
//            {
//                _step++;
//            }
//            break;
//
//            // Suffix
//        case 7:
//            packet.suffix = b;
//            _step = 0;
//            return true;
//        }
//    }
//    return false;
//}