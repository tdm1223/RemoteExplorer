#include "NetworkSession.h"
#include <WS2tcpip.h>

NetworkSession::NetworkSession(SOCKET socket, SOCKADDR_IN address) : socket_(socket), address_(address)
{
    InetNtopA(address_.sin_family, &address_.sin_addr, ipAddress_, 14);
}

SOCKET NetworkSession::GetSocket() const
{
    return socket_;
}

SOCKADDR_IN NetworkSession::GetAddress() const
{
    return address_;
}

const char* NetworkSession::GetIpAddress() const
{
    return ipAddress_;
}
