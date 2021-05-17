#ifndef NETWORK_NETWORKSESSION_H
#define NETWORK_NETWORKSESSION_H

#include <WinSock2.h>

class NetworkSession
{
public:
    NetworkSession(SOCKET socket, SOCKADDR_IN address);
    ~NetworkSession() = default;

    NetworkSession(const NetworkSession&) = delete;
    NetworkSession& operator=(const NetworkSession&) = delete;

    SOCKET GetSocket() const;
    SOCKADDR_IN GetAddress() const;
    const char* GetIpAddress() const;

private:
    SOCKET socket_;
    SOCKADDR_IN address_;
    char ipAddress_[14];
};

#endif