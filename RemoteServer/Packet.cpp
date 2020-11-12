#include "Packet.h"
//#include "Packet.h"
//
//Packet::Packet()
//{
//    command = 0;
//    size = 0;
//}
//
//Packet::~Packet()
//{}
//
//void Packet::WriteWithoutData(const int command, const int size, int& offset)
//{
//    int dataSize = sizeof(command) + sizeof(size);
//
//    buf.resize(dataSize);
//
//    memcpy(&buf[offset], command, sizeof(command));
//    offset += sizeof(command);
//    
//    memcpy(&buf[offset], size, sizeof(size));
//    offset += sizeof(size);
//
//
//    buf.push_back(prefix);
//    buf.push_back(command & 0xFF);
//    buf.push_back((command >> 8) & 0xFF);
//    buf.push_back((command >> 16) & 0xFF);
//    buf.push_back((command >> 24) & 0xFF);
//
//    buf.push_back(size & 0xFF);
//    buf.push_back((size >> 8) & 0xFF);
//    buf.push_back((size >> 16) & 0xFF);
//    buf.push_back((size >> 24) & 0xFF);
//
//    offset += sizeof(char) + sizeof(int) * 2;
//}
//
//void Packet::WriteData(char* data, int size, int& offset)
//{
//    offset += size;
//    for (int i = 0; i < size; i++)
//    {
//        buf.push_back(data[i]);
//    }
//}
//
//void Packet::Serialize(char* data)
//{
//    memcpy(data, &buf[0], buf.size());
//}
//
///// <summary>
///// ��Ŷ �����ϴ� �Լ�
///// </summary>
///// <param name="buffer">��Ŷ ����</param>
///// <param name="data">������</param>
///// <param name="command">��� ����</param>
///// <param name="size">������ ����</param>
///// <param name="offset">��Ŷ�� �� ����</param>
//void Packet::Build(char* buffer, int command, int size, char* data, int& offset)
//{
//    WriteWithoutData(command, size, offset);
//    WriteData(data, size, offset);
//    Serialize(buffer);
//}
//
//int Packet::GetHeaderSize()
//{
//    return 2 * sizeof(int) + sizeof(char);
//}