#include<iostream>
#include<WinSock2.h>
#include<vector>
#include"PacketCommand.h"
#include"UploadCommand.h"
#include"DownloadCommand.h"
#include"Client.h"
using namespace std;

int main()
{
    Client client;

    size_t com;
    while (true)
    {
        cout << "1 : Upload" << endl;
        cout << "2 : Download" << endl;
        cout << "3 : End" << endl;

        cout << "커맨드 입력 : ";
        cin >> com;
        Receiver receiver;
        
    }
    /*while (true)
    {



        if (com <= v.size())
        {
            if (v[com - 1]->Execute())
            {
                for (auto& t : v)
                {
                    if (t != v[com - 1])
                    {
                        t->Execute();
                    }
                }
            }
            else
            {
                cout << "return false" << endl;
            }
        }
        else
        {
            cout << "종료" << endl;
            break;
        }
    }*/
    return 0;
}