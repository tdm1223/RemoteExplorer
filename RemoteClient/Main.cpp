#include"Client.h"

int main()
{
    // 초기화하여 실행
    Client* client = new Client();
    if (client->Initialize())
    {
        client->Start();
    }
    return 0;
}