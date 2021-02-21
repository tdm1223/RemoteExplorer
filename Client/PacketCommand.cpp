#include "PacketCommand.h"

PacketCommand::PacketCommand()
{
}

PacketCommand::~PacketCommand()
{

}

const bool PacketCommand::OnParse(const char* buffer, const unsigned int bufferSize)
{
    return true;
}

const bool PacketCommand::OnBuild(char* buffer, unsigned int& buildBufferSize)
{
    return true;
}

const void PacketCommand::Clear()
{

}

const unsigned int PacketCommand::GetHeaderSize()
{
    return 0;
}

void PacketCommand::SetCommand(int command)
{
    this->command = command;
}

void PacketCommand::SetSize(int size)
{
    this->size = size;
}

void PacketCommand::SetData(std::vector<char> data)
{
    this->data = data;
}

const int PacketCommand::GetCommand() const
{
    return command;
}

const int PacketCommand::GetSize() const
{
    return size;
}

const std::string PacketCommand::GetPrefix() const
{
    return prefix;
}

const std::vector<char> PacketCommand::GetData() const
{
    return data;
}
