//#include<winsock2.h>
//#include<stdlib.h>
//#include<stdio.h>
//#include<thread>
//#include<string>
//#include<iostream>
//#include<queue>
//#include<chrono>
//#include<filesystem>
//#include"Packet.h"
//
//namespace fs = std::filesystem;
//
//#pragma comment(lib, "ws2_32")
//
//#pragma warning (disable:4996)
//#pragma warning (disable:6385)
//
//#define PORT 9000
//#define MAX_MSG_LEN 256
//#define SERVER_IP "127.0.0.1"
//#define BUF_SIZE 4096
//#define MESSAGE_SIZE 20
//
//enum msgType
//{
//    UPLOAD = 1,
//    DOWNLOAD = 2,
//    EXIT = 3,
//};
//
////파일 기본 정보
//struct Files
//{
//    char name[MAX_MSG_LEN];
//    size_t nameLen = 0;
//    int size;
//};
//
//void SendProc(SOCKET s);
//void RecvProc(SOCKET s);
//
//int main()
//{
//    std::queue<Files> sendQueue;
//    std::queue<Files> recvQueue;
//    // 윈속 초기화
//    WSADATA wsadata;
//    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
//    {
//        return -1;
//    }
//
//    // 서버와 통신용 소켓 생성
//    SOCKET sock;
//    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//    if (sock == -1)
//    {
//        return -1;
//    }
//
//    SOCKADDR_IN serverAddress;
//    serverAddress.sin_family = AF_INET;
//    serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
//    serverAddress.sin_port = htons(PORT);
//
//    // 연결 요청
//    if (connect(sock, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
//    {
//        return -1;
//    }
//
//    while (true)
//    {
//        std::cout << "1 : 업로드" << std::endl << "2 : 다운로드" << std::endl << "3 : 종료" << std::endl;
//        int type;
//        std::cin >> type;
//        if (type == UPLOAD)
//        {
//            std::thread sendThread(SendProc, sock);
//            sendThread.join();
//        }
//        else if (type == DOWNLOAD)
//        {
//            std::thread recvThread(RecvProc, sock);
//            recvThread.join();
//        }
//        else if (type == EXIT)
//        {
//            std::cout << "연결 종료" << std::endl;
//            break;
//        }
//        else
//        {
//            std::cout << "입력값이 유효하지 않습니다." << std::endl;
//            continue;
//        }
//    }
//    closesocket(sock);
//    WSACleanup(); // 윈속 해제화
//    return 0;
//}
//
//void SendProc(SOCKET s)
//{
//    std::cout << "send proc" << std::endl;
//    Packet test;
//    SOCKET sock = s;
//
//    std::string fileName;
//    Files sendFile;
//    FILE* fp;
//
//    std::cout << "현재 폴더에 있는 파일" << std::endl;
//    std::cout << "======================" << std::endl;
//    std::string files;
//    for (const fs::directory_entry& entry : fs::directory_iterator(fs::current_path()))
//    {
//        files = entry.path().string();
//        size_t pos = files.rfind("\\");
//        std::cout << files.substr(pos + 1) << std::endl;
//    }
//    std::cout << "======================" << std::endl;
//    fileName.clear();
//
//    std::cout << "업로드할 파일을 입력하세요" << std::endl;
//    std::cin >> fileName;
//    strcpy(sendFile.name, fileName.c_str());
//    sendFile.nameLen = strlen(sendFile.name);
//    fp = fopen(sendFile.name, "rb");
//    if (fp == NULL)
//    {
//        std::cout << "파일이 없습니다. 파일명을 확인하세요" << std::endl;
//    }
//    else
//    {
//        // 파일 끝으로 위치 옮김
//        fseek(fp, 0L, SEEK_END);
//
//        // 파일 크기 얻음
//        sendFile.size = ftell(fp);
//
//        // 다시 파일 처음으로 위치 옮김
//        fseek(fp, 0L, SEEK_SET);
//        fclose(fp);
//
//        std::cout << "큐에 넣는 파일명 : " << sendFile.name << " 전송하는 파일 크기 : " << sendFile.size << " Byte" << std::endl;
//
//        std::vector<char> buffer;
//
//        Packet fileInfoPacket;
//        fileInfoPacket.prefix = 0x7F;
//        fileInfoPacket.command = UPLOAD;
//        for (auto tmp : fileName)
//        {
//            fileInfoPacket.data.push_back(tmp);
//        }
//        fileInfoPacket.size = fileInfoPacket.data.size();
//        fileInfoPacket.serialize(buffer);
//
//        std::cout << buffer.size() << std::endl;
//        send(sock, &buffer[0], buffer.size(), 0);
//
//        // 업로드라는것을 알림
//        //send(sock, (char*)&type, sizeof(int), 0);
//        //std::cout << "업로드 요청" << std::endl;
//
//        
//        // 헤더 만듦
//        //char header[BUF_SIZE];
//        //memcpy(header, &sendFile.size, sizeof(sendFile.size));
//        //memcpy(header + sizeof(sendFile.size), &sendFile.name, sizeof(sendFile.name));
//        //send(sock, header, BUF_SIZE, 0);
//
//        //// 데이터 통신에 사용할 변수
//        //int sendSize = 0;
//        //fp = fopen(sendFile.name, "rb");
//        //char buf[BUF_SIZE];
//        //// 파일 전송
//        //while (1)
//        //{
//        //    sendSize = fread(buf, 1, BUF_SIZE, fp);
//        //    if (sendSize < BUF_SIZE)
//        //    {
//        //        send(sock, buf, sendSize, 0);
//        //        break;
//        //    }
//        //    send(sock, buf, BUF_SIZE, 0);
//        //}
//        //fclose(fp);
//        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//        //std::cout << "전송 완료 : " << sendFile.name << std::endl;
//    }
//}
//
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

#include<WinSock2.h>
#include<string>
#include<iostream>
#include"Packet.h"
#include<filesystem>

namespace fs = std::filesystem;

#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 4096

// 메세지 타입
#define UPLOAD 1
#define DOWNLOAD 2
#define END 3

int main()
{
    // 대기 버퍼에서 헤더를 분석할 순서 인지를 판별하기 위한 변수
    bool isHeader = false;

    // client 종료
    bool isExit = false;

    // 현재 대기 버퍼에 있는 데이터 길이
    unsigned int curLen = 0;

    //받은 메세지 길이
    unsigned int receivedMsgLen = 0;

    // 보낼 메세지 버퍼
    char msgBuffer[BUF_SIZE] = { 0, };

    // 받은 메세지 대기 버퍼
    char waitBuffer[BUF_SIZE] = { 0, };

    // receive 버퍼
    char recvBuffer[BUF_SIZE] = { 0, };

    // send 버퍼
    char sendBuffer[BUF_SIZE] = { 0, };

    // 받은 헤더 처리 버퍼
    char headerBuffer[BUF_SIZE] = { 0, };

    // 받은 메세지 처리 버퍼
    char recvMsgBuffer[BUF_SIZE] = { 0, };

    // 대기 버퍼 스왑용 버퍼
    char tmpBuffer[BUF_SIZE] = { 0, };

    // send 버퍼 인덱스 포인터
    char* sendBufferOffset = 0;

    // 대기 버퍼 인덱스 포인터
    char* recvBufferIndex = 0;

    // data 정리용 포인터 선언
    char* dataSortIndex = 0;

    // 메세지 헤더 구조체 선언
    Packet packet;

    // receive 헤더 구조체 포인터 선언
    Packet recvHeader;

    // client 소켓 선언
    SOCKET clientSock;

    WSADATA wsadata;
    // 서버주소 구조체 변수 선언
    SOCKADDR_IN serverAddress;

    // 소켓을 초기화
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

    recvBufferIndex = waitBuffer;

    while (true)
    {
        // message type을 입력 받음
        std::cout << "1 - UPLOAD" << std::endl;
        std::cout << "2 - DOWNLOAD" << std::endl;
        std::cout << "3 - END" << std::endl;
        std::cin >> packet.command;

        if (packet.command == UPLOAD)
        {
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
            strcpy(msgBuffer, fileName.c_str());

            // 인덱스 초기화
            sendBufferOffset = sendBuffer;

            // 헤더 저장
            memcpy(sendBufferOffset, &packet, sizeof(packet));

            // 인덱스에 헤더 크기 추가
            sendBufferOffset += sizeof(packet);

            // 메시지 저장
            memcpy(sendBufferOffset, msgBuffer, packet.size);

            // 메세지 전송
            if (send(clientSock, sendBuffer, sizeof(packet) + packet.size, 0) < 0)
            {
                std::cout << "send error!!" << std::endl;
                return 0;
            }

            // 메시지 전송 후 수신을 기다림
            if (packet.command == UPLOAD)
            {
                // 메세지 수신
                while (true)
                {
                    memset(recvBuffer, 0, sizeof(recvBuffer));

                    // 메세지 수신
                    receivedMsgLen = recv(clientSock, recvBuffer, BUF_SIZE, 0);

                    std::cout << "receivedMsgLen : " << receivedMsgLen << std::endl;
                    // 수신한 메시지가 존재
                    if (receivedMsgLen >= 0)
                    {
                        // 받은 메시지를 버퍼에 저장
                        memcpy(recvBufferIndex, recvBuffer, receivedMsgLen);

                        // 현재 위치, 받은 버퍼 인덱스는 받은 메시지의 길이만큼 증가
                        curLen += receivedMsgLen;
                        recvBufferIndex += receivedMsgLen;
                        receivedMsgLen = 0;

                        // 헤더를 분석
                        if (!isHeader)
                        {
                            // 헤더 길이 이상의 data가 있는지 확인
                            if (curLen >= sizeof(packet))
                            {
                                // 대기 버퍼를 받아서 headerBuffer에 저장
                                memset(headerBuffer, 0, sizeof(headerBuffer));
                                memcpy(headerBuffer, waitBuffer, sizeof(packet));
                                headerBuffer[sizeof(packet)] = '\0';

                                // headerBuffer에 저장한것을 recvHeader에 저장
                                memcpy(&recvHeader, headerBuffer, sizeof(recvHeader));

                                // 버퍼 정리
                                dataSortIndex = waitBuffer + sizeof(recvHeader);
                                memset(tmpBuffer, 0, sizeof(tmpBuffer));
                                memcpy(tmpBuffer, dataSortIndex, sizeof(waitBuffer) - sizeof(recvHeader));

                                memset(waitBuffer, 0, sizeof(waitBuffer));
                                memcpy(waitBuffer, tmpBuffer, sizeof(tmpBuffer));

                                curLen -= sizeof(recvHeader);
                                recvBufferIndex -= sizeof(recvHeader);
                                isHeader = true;
                            }
                        }

                        if (isHeader)
                        {
                            if (curLen >= recvHeader.size)
                            {
                                memset(recvMsgBuffer, 0, sizeof(recvMsgBuffer));
                                memcpy(recvMsgBuffer, waitBuffer, recvHeader.size);
                                recvMsgBuffer[recvHeader.size] = '\0';

                                dataSortIndex = waitBuffer + recvHeader.size;
                                memset(tmpBuffer, 0, sizeof(tmpBuffer));
                                memcpy(tmpBuffer, dataSortIndex, sizeof(waitBuffer) - recvHeader.size);

                                memset(waitBuffer, 0, sizeof(waitBuffer));
                                memcpy(waitBuffer, tmpBuffer, sizeof(tmpBuffer));

                                curLen -= recvHeader.size;
                                recvBufferIndex -= recvHeader.size;
                                isHeader = false;

                                if (recvHeader.command == UPLOAD)
                                {
                                    std::cout << "message length is " << recvHeader.size << "byte" << std::endl;
                                    std::cout << "receive message : " << recvMsgBuffer << std::endl;
                                    break;
                                }
                                else
                                {
                                    std::cout << "recv incorrect message" << std::endl;
                                }
                            }
                        }
                    }
                    else
                    {
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