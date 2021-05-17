#include "ListCommand.h"

bool ListCommand::Execute(SOCKET sock, void* buffer)
{
    // COMMAND SEND
    memset((char*)buffer, 0, Util::kBufferSize);
    if (!SendCommand(sock, (char*)buffer, Util::COMMAND::LIST))
    {
        return false;
    }

    // RECV
    int cnt = 0;
    if (!RecvLength(sock, &cnt))
    {
        std::cout << "RECV LENGTH ERROR" << std::endl;
        return false;
    }

    // 요청해서 파일 리스트 보여주는 코드
    std::cout << "파일 개수 : " << cnt << std::endl;
    char recvBuf[Util::kMaxFileNameLength];
    for (int i = 0; i < cnt; i++)
    {
        int nameLen = 0;
        if (!Recv(sock, recvBuf, &nameLen))
        {
            std::cout << "RECV DATA ERROR" << std::endl;
            return false;
        }
        else
        {
            std::cout << recvBuf << std::endl;
        }
    }

    return true;
}
