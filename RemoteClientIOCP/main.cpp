#include <iostream>
#include "IOCPClient.h"

int main()
{
    IOCPClient* client = new IOCPClient();
    if (client->Connect("127.0.0.1", 9000))
    {
        while (TRUE)
        {
            int command;
            std::cout << "1 - ECHO" << std::endl;
            std::cout << "2 - DOWNLOAD" << std::endl;
            std::cout << "3 - LIST" << std::endl;
            std::cout << "4 - END" << std::endl;
            std::cin >> command;

            if (!client->Query(command))
            {
                break;
            }
        }
    }
    return 0;
}