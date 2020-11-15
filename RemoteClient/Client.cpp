#include "Client.h"

Client::Client()
{
    // ������ �ʱ�ȭ
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        exit(1);
    }

    // client ���� ����
    clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (clientSock == INVALID_SOCKET)
    {
        WSACleanup();
        exit(1);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));

    // ���� �ּ� ����
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(PORT);

    // ������ ���� ��û 
    if (connect(clientSock, (LPSOCKADDR)&serverAddress, sizeof(serverAddress)) != 0)
    {
        exit(1);
    }
}

void Client::Start()
{
    while (true)
    {
        // message type�� �Է� ����
        int command;
        std::cout << "1 - UPLOAD" << std::endl;
        std::cout << "2 - DOWNLOAD" << std::endl;
        std::cout << "3 - END" << std::endl;
        std::cin >> command;

        packet.SetCommand(command);
        const char* prefix = "ESTSOFT";
        packet.SetPrefix(prefix);
        if (packet.GetCommand() == UPLOAD)
        {
            // receive ����
            char buffer[BUF_SIZE];

            // ���� ������ �ִ� ���� ���
            std::cout << "���� ������ �ִ� ����" << std::endl;
            std::cout << "======================" << std::endl;
            std::string files;
            for (const fs::directory_entry& entry : fs::directory_iterator(fs::current_path()))
            {
                files = entry.path().string();
                size_t pos = files.rfind("\\");
                std::cout << files.substr(pos + 1) << std::endl;
            }
            std::cout << "======================" << std::endl;

            // ���ϸ� �Է� ����
            std::string fileName = "";
            std::cout << "���ϸ� : ";
            std::cin >> fileName;

            std::vector<char> data(fileName.begin(), fileName.end());
            packet.SetData(data);

            FILE* fp;
            fp = fopen(fileName.c_str(), "rb");
            if (fp == NULL)
            {
                std::cout << "���� �����Դϴ�." << std::endl;
            }
            else
            {
                // ���ϸ��� ���� ����
                packet.SetSize(fileName.length());

                // ��Ŷ ����
                unsigned int offset = 0;
                packet.OnBuild(buffer, offset);

                // �޼��� ����
                std::cout << "�����ϴ� �޽��� ũ�� : " << offset << std::endl;
                if (send(clientSock, buffer, offset, 0) < 0)
                {
                    exit(1);
                }

                while (true)
                {
                    packet.Clear();
                    memset(buffer, 0, sizeof(buffer));

                    // ���� ������ ��ġ �ű�
                    fseek(fp, 0L, SEEK_END);
                    // ���� ũ�� ����
                    int fileSize = ftell(fp);
                    std::string fileSizeString = std::to_string(fileSize);

                    packet.SetPrefix("ESTSOFT");
                    packet.SetCommand(UPLOAD);
                    packet.SetSize(fileSizeString.size());
                    std::vector<char> data(fileSizeString.begin(), fileSizeString.end());
                    packet.SetData(data);

                    // �ٽ� ���� ó������ ��ġ �ű�
                    fseek(fp, 0L, SEEK_SET);
                    fclose(fp);
                    std::cout << "�����ϴ� ���ϸ� : " << fileName << " �����ϴ� ���� ũ�� : " << packet.GetSize() << " Byte" << std::endl;

                    offset = 0;

                    packet.OnBuild(buffer, offset);
                    send(clientSock, buffer, offset, 0);

                    // ������ ��ſ� ����� ����
                    int sendSize = 0;
                    fp = fopen(fileName.c_str(), "rb");

                    int totalSize = 0;

                    char sendData[4082];

                    const int dataSize = BUF_SIZE - packet.GetHeaderSize();
                    // ���� ����
                    while (1)
                    {
                        offset = 0;
                        packet.Clear();
                        sendSize = fread(sendData, 1, dataSize, fp);

                        packet.SetPrefix("ESTSOFT");
                        packet.SetCommand(UPLOAD);
                        packet.SetSize(sendSize);
                        std::cout << sendSize << std::endl;

                        data.clear();
                        for (int i = 0; i < sendSize; i++)
                        {
                            data.push_back(sendData[i]);
                        }
                        packet.SetData(data);
                        packet.OnBuild(buffer, offset);
                        totalSize += sendSize;
                        if (sendSize < BUF_SIZE - packet.GetHeaderSize())
                        {
                            send(clientSock, buffer, offset, 0);
                            break;
                        }
                        send(clientSock, buffer, BUF_SIZE, 0);
                    }
                    fclose(fp);
                    break;
                }
            }
        }
        else if (packet.GetCommand() == DOWNLOAD)
        {

        }
        else if (packet.GetCommand() == END)
        {
            break;
        }
    }
    // ������ ����
    closesocket(clientSock);

    std::cout << "client exit" << std::endl;
}
