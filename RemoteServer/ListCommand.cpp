#include"ListCommand.h"

void ListCommand::Execute(SOCKET& sock)
{
    std::cout << "LIST" << std::endl;

    std::string msg = "recv LIST command";
    char buf[kBufSize];
    memset(buf, 0, kBufSize);

    strcpy_s(buf, msg.c_str());

    char msgLength[4];
    memset(msgLength, 0, 4);
    SerializeInt(msg.length(), msgLength);

    send(sock, msgLength, 4, false);
    send(sock, buf, msg.length(), false);
}