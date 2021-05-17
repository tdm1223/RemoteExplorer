#include"IOCPServer.h"

int main()
{
    IOCPServer* server = new IOCPServer();
    if (server->Start())
    {
        while (1);
    }

    return 0;
}
