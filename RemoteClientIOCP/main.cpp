#include <iostream>
#include "IOCPClient.h"

int main()
{
    IOCPClient* client = new IOCPClient();
    client->Connect("127.0.0.1", 9000);
    int k;
    while (1)
    {
        std::cout << "입력 : ";
        std::cin >> k;
        if (k == 1)
        {
            std::string s;
            std::cin >> s;
            if (client->Query(s.c_str()))
            {
                std::cout << "쿼리 종료" << std::endl;
            }
        }
        else if (k == 0)
        {
            break;
        }
    }
    return 0;
}
