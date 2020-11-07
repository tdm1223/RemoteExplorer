#include<WinSock2.h>
#include<string>
#include<iostream>
#include"Packet.h"
#include<filesystem>
#include"Parser.h"

namespace fs = std::filesystem;

#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 4096

// 메세지 타입
#define UPLOAD 1
#define DOWNLOAD 2
#define END 3

int main()
{
    // receive 버퍼
    char buffer[BUF_SIZE];

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
    serverAddress.sin_port = htons(9000);

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

        // 보낼 데이터 버퍼
        char data[BUF_SIZE];

        if (packet.command == UPLOAD)
        {
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
            packet.size = fileName.length();
            strcpy(data, fileName.c_str());

            // 패킷 빌드
            int offset = 0;
            packet.Build(buffer, UPLOAD, fileName.length(), data, offset);

            // 메세지 전송
            std::cout << "전송하는 메시지 크기 : " << offset << std::endl;
            if (send(clientSock, buffer, offset, 0) < 0)
            {
                std::cout << "send error!!" << std::endl;
                return 0;
            }

            // 보낸 버퍼 초기화
            memset(buffer, 0, BUF_SIZE);

            // 메시지 전송 후 수신을 기다림
            if (packet.command == UPLOAD)
            {
                while (true)
                {
                    memset(buffer, 0, sizeof(buffer));
                    Packet result;

                    int recvLen = recv(clientSock, buffer, BUF_SIZE, 0);
                    std::cout << "서버로 부터 받은 메시지 크기 : " << recvLen << std::endl;
                    int byteRead = 0;
                    Parser parser;
                    parser.Parsing(buffer, recvLen, result);

                    if (result.command == UPLOAD)
                    {
                        std::cout << "받은 명령 : " << result.command << std::endl;
                        std::cout << "받은 메시지 크기 : " << result.size << std::endl;
                        std::cout << "받은 메시지 : ";
                        for (auto tmp : result.buf)
                        {
                            std::cout << tmp;
                        }
                        std::cout << std::endl;
                        break;
                    }
                }
            }
            else if (packet.command == DOWNLOAD)
            {

            }
            else if (packet.command == END)
            {
                break;
            }
        }
    }
    // 소켓을 닫음
    closesocket(clientSock);

    std::cout << "client exit" << std::endl;
    return 0;
}