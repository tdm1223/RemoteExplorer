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
        char data[BUF_SIZE - sizeof(char) - 2 * sizeof(int)];

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

            FILE* fp;
            fp = fopen(fileName.c_str(), "rb");
            if (fp == NULL)
            {
                std::cout << "없는 파일입니다." << std::endl;
                continue;
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
                if (send(clientSock, buffer, offset, 0) < 0)
                {
                    return 0;
                }

                // 메시지 전송 후 수신을 기다림
                if (packet.command == UPLOAD)
                {
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
                        std::cout << "큐에 넣는 파일명 : " << fileName << " 전송하는 파일 크기 : " << packet.size << " Byte" << std::endl;

                        offset = 0;
                        packet.buf.clear();
                        int size = sprintf(data, "%d", packet.size);
                        //std::cout << "size : "<<size << std::endl;
                        packet.Build(buffer, UPLOAD, size, data, offset);
                        send(clientSock, buffer, offset, 0);

                        // 데이터 통신에 사용할 변수
                        int sendSize = 0;
                        fp = fopen(fileName.c_str(), "rb");

                        int totalSize = 0;
                        // 파일 전송
                        while (1)
                        {
                            Packet filePacket;
                            offset = 0;
                            sendSize = fread(data, 1, BUF_SIZE - sizeof(char) - 2 * sizeof(int), fp);
                            filePacket.Build(buffer, UPLOAD, sendSize, data, offset);
                            totalSize += sendSize;
                            //std::cout << "sendSize : " << sendSize << " totalSize : " << totalSize << std::endl;
                            if (sendSize < BUF_SIZE - filePacket.GetHeaderSize())
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

//void RecvProc(SOCKET s)
//{
//    SOCKET sock = s;
//
//    // 다운로드라는것을 알림
//    int type = 2;
//    send(sock, (char*)&type, sizeof(int), 0);
//
//    std::cout << "업로드 요청" << std::endl;
//    std::cout << "원격 폴더에 있는 파일" << std::endl;
//    std::cout << "======================" << std::endl;
//    int cnt = 0;
//    recv(sock, (char*)&cnt, sizeof(int), 0);
//
//    std::cout << "파일 개수 : " << cnt << std::endl;
//    std::cout << "다운로드할 파일을 입력하세요" << std::endl;
//    // 요청해서 파일 리스트 보여주는 코드
//    char recvBuf[BUF_SIZE];
//    for (int i = 0; i < cnt; i++)
//    {
//        recv(sock, (char*)&recvBuf, MAX_MSG_LEN, 0);
//        std::cout << recvBuf << std::endl;
//    }
//    std::cout << "======================" << std::endl;
//    std::cout << "파일명을 입력하세요" << std::endl;
//
//    // 다운 받으려는 파일명을 보냄
//    std::string fileName;
//    std::cin >> fileName;
//    strcpy(recvBuf, fileName.c_str());
//    send(sock, (char*)&recvBuf, MAX_MSG_LEN, 0);
//
//    // 다운 받으려는 파일 크기를 받음
//    int fileSize = 0;
//    recv(sock, (char*)&fileSize, sizeof(fileSize), 0);
//    std::cout << "받으려는 파일 크기 : " << fileSize << std::endl;
//
//    // 파일 다운로드
//    std::cout << "파일 다운로드 시작" << std::endl;
//    FILE* fp = fopen(fileName.c_str(), "wb+");
//    if (fp == NULL)
//    {
//        std::cout << "파일 쓰기 오류. 해당 파일 생략" << std::endl;
//        return;
//    }
//
//    memset(recvBuf, 0, sizeof(recvBuf));
//    int readSize = 0;
//    int totalSize = 0;
//    while ((readSize = recv(sock, recvBuf, BUF_SIZE, 0)) != 0)
//    {
//        std::cout << "total : " << totalSize << std::endl;
//        if (GetLastError() == WSAEWOULDBLOCK)
//        {
//            Sleep(50); // 잠시 기다렸다가 재전송
//            if (totalSize == fileSize)
//            {
//                std::cout << "파일 받기 완료" << std::endl;
//                break;
//            }
//            continue;
//        }
//        totalSize += readSize;
//        fwrite(recvBuf, 1, readSize, fp);
//        if (totalSize == fileSize)
//        {
//            std::cout << "파일 받기 완료" << std::endl;
//            break;
//        }
//    }
//    fclose(fp);
//}