#include"Client.h"

int main()
{
    Client* client = new Client();
    if (client->Initialize())
    {
        client->Loop();
    }
    return 0;
}