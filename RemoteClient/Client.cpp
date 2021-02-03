#include "Client.h"

Client::Client()
{
    // 소켓을 초기화
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        exit(1);
    }

    // client 소켓 생성
    clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (clientSock == INVALID_SOCKET)
    {
        WSACleanup();
        exit(1);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));

    // 서버 주소 지정
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(PORT);

    // 서버에 접속 요청 
    if (connect(clientSock, (LPSOCKADDR)&serverAddress, sizeof(serverAddress)) != 0)
    {
        exit(1);
    }
}

void Client::Initialize()
{
    EventLoop();

    // 소켓을 닫음
    closesocket(clientSock);
    std::cout << "client exit" << std::endl;
}

void Client::EventLoop()
{
    while (true)
    {
        // message type을 입력 받음
        int command;
        std::cout << "1 - UPLOAD" << std::endl;
        std::cout << "2 - DOWNLOAD" << std::endl;
        std::cout << "3 - END" << std::endl;
        std::cin >> command;

        // 명령을 패킷에 저장
        packet.SetCommand(command);
        const char* prefix = "ESTSOFT";

        // prefix를 패킷에 저장
        packet.SetPrefix(prefix);
        if (packet.GetCommand() == UPLOAD)
        {
            Upload();
        }
        else if (packet.GetCommand() == DOWNLOAD)
        {
            break;
        }
        else if (packet.GetCommand() == END)
        {
            return;
        }
    }
}

void Client::Upload()
{
    // send 버퍼 초기화
    memset(sendBuffer, 0, sizeof(sendBuffer));

    // 현재 폴더에 있는 파일 출력
    std::cout << "현재 폴더에 있는 파일" << std::endl;
    std::cout << "======================" << std::endl;
    std::string files;
    for (const fs::directory_entry& entry : fs::directory_iterator(fs::current_path()))
    {
        files = entry.path().string();
        size_t pos = files.rfind("\\");
        std::cout << files.substr(pos + 1) << std::endl;
    }
    std::cout << "======================" << std::endl;

    // 파일명 입력 받음
    std::string fileName = "";
    std::cout << "파일명 : ";
    std::cin >> fileName;

    // 파일명을 패킷에 저장
    std::vector<char> data(fileName.begin(), fileName.end());
    packet.SetData(data);

    FILE* fp;
    fp = fopen(fileName.c_str(), "rb");
    if (fp == NULL)
    {
        std::cout << "없는 파일입니다." << std::endl;
    }
    else
    {
        // 파일 크기를 패킷에 저장
        packet.SetSize(fileName.length());

        // 패킷을 빌드 하여 버퍼에 저장
        unsigned int offset = 0;
        packet.OnBuild(sendBuffer, offset);

        // 메세지 전송
        std::cout << "전송하는 메시지 크기 : " << offset << std::endl;
        if (send(clientSock, sendBuffer, offset, 0) < 0)
        {
            exit(1);
        }

        packet.Clear();
        memset(sendBuffer, 0, sizeof(sendBuffer));

        // 파일 끝으로 위치 옮김
        fseek(fp, 0L, SEEK_END);
        // 파일 크기 얻음
        int fileSize = ftell(fp);
        std::string fileSizeString = std::to_string(fileSize);

        packet.SetPrefix("ESTSOFT");
        packet.SetCommand(UPLOAD);
        packet.SetSize(fileSizeString.size());
        std::vector<char> data(fileSizeString.begin(), fileSizeString.end());
        packet.SetData(data);

        // 다시 파일 처음으로 위치 옮김
        fseek(fp, 0L, SEEK_SET);
        fclose(fp);
        std::cout << "전송하는 파일명 : " << fileName << " 전송하는 파일 크기 : " << packet.GetSize() << " Byte" << std::endl;

        // 전송 관련 변수 초기화
        offset = 0;

        // 패킷 빌드
        packet.OnBuild(sendBuffer, offset);
        send(clientSock, sendBuffer, offset, 0);

        // 데이터 통신에 사용할 변수
        int sendSize = 0;
        fp = fopen(fileName.c_str(), "rb");
        int totalSize = 0;
        char sendData[4082];

        // 파일 전송
        while (1)
        {
            memset(sendData, 0, sizeof(sendData));
            offset = 0;
            packet.Clear();
            sendSize = fread(sendData, 1, dataSize, fp);

            // 파일을 담은 패킷을 보낼때 사전 작업
            packet.SetPrefix("ESTSOFT");
            packet.SetCommand(UPLOAD);
            packet.SetSize(sendSize);

            data.clear();
            for (int i = 0; i < sendSize; i++)
            {
                data.push_back(sendData[i]);
            }
            packet.SetData(data);
            packet.OnBuild(sendBuffer, offset);
            totalSize += sendSize;
            if (sendSize < BUF_SIZE - packet.GetHeaderSize())
            {
                send(clientSock, sendBuffer, offset, 0);
                break;
            }
            send(clientSock, sendBuffer, BUF_SIZE, 0);
        }
        fclose(fp);
    }
}