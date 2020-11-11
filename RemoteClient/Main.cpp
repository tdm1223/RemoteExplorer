#include<WinSock2.h>
#include<string>
#include<iostream>
#include"Packet.h"
#include<filesystem>
#include"Parser.h"
#include <thread>

namespace fs = std::filesystem;

#pragma comment(lib, "ws2_32.lib")

enum
{
    UPLOAD = 1,
    DOWNLOAD = 2,
    END = 3,
    BUF_SIZE = 4096,
    PORT = 9000
};

void FileUpload(SOCKET s);

int main()
{
    // client 소켓 선언
    SOCKET clientSock;

    // 서버주소 구조체 변수 선언
    SOCKADDR_IN serverAddress;

    // 소켓을 초기화
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        return -1;
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

    while (true)
    {
        // 헤더 구조체 선언
        Packet packet;

        // message type을 입력 받음
        std::cout << "1 - UPLOAD" << std::endl;
        std::cout << "2 - DOWNLOAD" << std::endl;
        std::cout << "3 - END" << std::endl;
        std::cin >> packet.command;

        if (packet.command == UPLOAD)
        {
            std::thread uploadThread([=] {FileUpload(clientSock); });
            uploadThread.join();
        }
        else if (packet.command == DOWNLOAD)
        {

        }
        else if (packet.command == END)
        {
            break;
        }
    }
    // 소켓을 닫음
    closesocket(clientSock);

    std::cout << "client exit" << std::endl;
    return 0;
}

void FileUpload(SOCKET s)
{
    Packet packet;
    packet.command = UPLOAD;
    SOCKET sock = s;

    // receive 버퍼
    char buffer[BUF_SIZE];

    // 보낼 데이터 버퍼
    char data[BUF_SIZE - sizeof(char) - 2 * sizeof(int)];

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

    FILE* fp;
    fp = fopen(fileName.c_str(), "rb");
    if (fp == NULL)
    {
        std::cout << "없는 파일입니다." << std::endl;
        return;
    }
    else
    {
        // 파일명을 먼저 전달
        packet.size = fileName.length();
        strcpy(data, fileName.c_str());

        // 패킷 빌드
        int offset = 0;
        packet.Build(buffer, UPLOAD, fileName.length(), data, offset);

        // 메세지 전송
        std::cout << "전송하는 메시지 크기 : " << offset << std::endl;
        if (send(sock, buffer, offset, 0) < 0)
        {
            return;
        }

        while (true)
        {
            memset(buffer, 0, sizeof(buffer));

            // 파일 끝으로 위치 옮김
            fseek(fp, 0L, SEEK_END);

            // 파일 크기 얻음
            packet.size = ftell(fp);

            // 다시 파일 처음으로 위치 옮김
            fseek(fp, 0L, SEEK_SET);
            fclose(fp);
            std::cout << "전송하는 파일명 : " << fileName << " 전송하는 파일 크기 : " << packet.size << " Byte" << std::endl;

            offset = 0;
            packet.buf.clear();
            int size = sprintf(data, "%d", packet.size);
            //std::cout << "size : "<<size << std::endl;
            packet.Build(buffer, UPLOAD, size, data, offset);
            send(sock, buffer, offset, 0);

            // 데이터 통신에 사용할 변수
            int sendSize = 0;
            fp = fopen(fileName.c_str(), "rb");

            int totalSize = 0;
            // 파일 전송
            while (1)
            {
                Packet filePacket;
                offset = 0;
                sendSize = fread(data, 1, BUF_SIZE - filePacket.GetHeaderSize(), fp);
                filePacket.Build(buffer, UPLOAD, sendSize, data, offset);
                totalSize += sendSize;
                //std::cout << "sendSize : " << sendSize << " totalSize : " << totalSize << std::endl;
                if (sendSize < BUF_SIZE - filePacket.GetHeaderSize())
                {
                    send(sock, buffer, offset, 0);
                    break;
                }
                send(sock, buffer, BUF_SIZE, 0);
            }
            fclose(fp);
            break;
        }
    }
    return;
}