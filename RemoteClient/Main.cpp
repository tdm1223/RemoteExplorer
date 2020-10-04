#include<winsock2.h>
#include<stdlib.h>
#include<stdio.h>
#include<thread>
#include<string>
#include<iostream>
#include<queue>
#include<chrono>
#include<filesystem>

namespace fs = std::filesystem;

#pragma comment(lib, "ws2_32")

#pragma warning (disable:4996)

#define PORT 9000
#define MAX_MSG_LEN 256
#define SERVER_IP "127.0.0.1"
#define BUF_SIZE 512
#define MESSAGE_SIZE 20

enum msgType
{
    UPLOAD = 1,
    DOWNLOAD = 2
};

//파일 기본 정보
struct Files
{
    char name[MAX_MSG_LEN];
    size_t nameLen = 0;
    unsigned int size;
};

void SendProc(SOCKET s, std::queue<Files>* q);
void RecvProc(SOCKET s, std::queue<Files>* q);

int main()
{
    std::queue<Files> sendQueue;
    std::queue<Files> recvQueue;
    // 윈속 초기화
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        return -1;
    }

    // 서버와 통신용 소켓 생성
    SOCKET sock;
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1)
    {
        return -1;
    }

    SOCKADDR_IN serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddress.sin_port = htons(PORT);

    // 연결 요청
    if (connect(sock, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
    {
        return -1;
    }

    std::string fileName;
    Files sendFile;
    FILE* fp;

    std::thread sendThread(SendProc, sock, &sendQueue);
    std::thread recvThread(RecvProc, sock, &recvQueue);

    while (true)
    {
        std::cout << "1 : 업로드" << std::endl << "2: 다운로드" << std::endl;
        int type;
        std::cin >> type;
        if (type == UPLOAD)
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
            fileName.clear();

            std::cout << "업로드할 파일을 입력하세요" << std::endl;
            std::cin >> fileName;
            strcpy(sendFile.name, fileName.c_str());
            sendFile.nameLen = strlen(sendFile.name);
            fp = fopen(sendFile.name, "rb");
            if (fp == NULL)
            {
                // 종료일 경우
                if (strcmp(sendFile.name, "exit") == 0)
                {
                    sendFile.size = 0;
                    sendQueue.push(sendFile);
                    break;
                }
                else if (strcmp(sendFile.name, "test") == 0)
                {
                    for (int i = 0; i < 5; i++)
                    {
                        Files file;
                        std::string name = std::to_string(i);
                        name += ".txt";
                        strcpy(file.name, name.c_str());
                        FILE* f = fopen(file.name, "rb");

                        // 파일 끝으로 위치 옮김
                        fseek(f, 0L, SEEK_END);

                        // 파일 크기 얻음
                        file.size = ftell(f);

                        // 다시 파일 처음으로 위치 옮김
                        fseek(f, 0L, SEEK_SET);
                        fclose(f);

                        sendQueue.push(file);
                    }
                }
                else if (strcmp(sendFile.name, "test2") == 0)
                {
                    for (int i = 5; i < 10; i++)
                    {
                        Files file;
                        std::string name = std::to_string(i);
                        name += ".txt";
                        strcpy(file.name, name.c_str());
                        FILE* f = fopen(file.name, "rb");

                        // 파일 끝으로 위치 옮김
                        fseek(f, 0L, SEEK_END);

                        // 파일 크기 얻음
                        file.size = ftell(f);

                        // 다시 파일 처음으로 위치 옮김
                        fseek(f, 0L, SEEK_SET);
                        fclose(f);

                        sendQueue.push(file);
                    }
                }
                else
                {
                    std::cout << "파일이 없습니다. 파일명을 확인하세요" << std::endl;
                    continue;
                }
            }
            else
            {
                // 파일 끝으로 위치 옮김
                fseek(fp, 0L, SEEK_END);

                // 파일 크기 얻음
                sendFile.size = ftell(fp);

                // 다시 파일 처음으로 위치 옮김
                fseek(fp, 0L, SEEK_SET);
                fclose(fp);

                std::cout << "큐에 넣는 파일명 : " << sendFile.name << " 전송하는 파일 크기 : " << sendFile.size << " Byte" << std::endl;
                sendQueue.push(sendFile);
            }
        }
        else if (type == DOWNLOAD)
        {
            // 다운로드라는것을 알림
            send(sock, (char*)&type, sizeof(int), 0);

            std::cout << "업로드 요청" << std::endl;
            std::cout << "원격 폴더에 있는 파일" << std::endl;
            std::cout << "======================" << std::endl;
            int cnt = 0;
            recv(sock, (char*)&cnt, sizeof(int), 0);

            std::cout << "파일 개수 : " << cnt << std::endl;
            std::cout << "다운로드할 파일을 입력하세요" << std::endl;
            // 요청해서 파일 리스트 보여주는 코드
            char recvBuf[256];
            for (int i = 0; i < cnt; i++)
            {
                recv(sock, (char*)&recvBuf, MAX_MSG_LEN, 0);
                std::cout << recvBuf << std::endl;
            }
            std::cout << "======================" << std::endl;
            std::cout << "파일명을 입력하세요" << std::endl;

            // 다운 받으려는 파일명을 보냄
            std::cin >> fileName;
            strcpy(recvBuf, fileName.c_str());
            send(sock, (char*)&recvBuf, MAX_MSG_LEN, 0);

            // 다운 받으려는 파일 크기를 받음
            int fileSize = 0;
            recv(sock, (char*)&fileSize, sizeof(fileSize), 0);
            std::cout << "받으려는 파일 크기 : " << fileSize << std::endl;

            // 파일 다운로드
            std::cout << "파일 다운로드 시작" << std::endl;
            FILE* fp = fopen(fileName.c_str(), "wb+");
            int readSize = 0;
            int totalSize = 0;
            char buf[BUF_SIZE];
            while ((readSize = recv(sock, buf, BUF_SIZE, 0)) != 0)
            {
                if (GetLastError() == WSAEWOULDBLOCK)
                {
                    Sleep(50); // 잠시 기다렸다가 재전송
                    if (totalSize == fileSize)
                    {
                        std::cout << "파일 받기 완료" << std::endl;
                        break;
                    }
                    continue;
                }
                totalSize += readSize;
                fwrite(buf, 1, readSize, fp);
                if (totalSize == fileSize)
                {
                    std::cout << "파일 받기 완료" << std::endl;
                    break;
                }
            }
            fclose(fp);
        }
        else
        {
            std::cout << "입력값이 유효하지 않습니다." << std::endl;
            continue;
        }
    }
    sendThread.join();
    recvThread.join();
    closesocket(sock);
    WSACleanup(); // 윈속 해제화
    return 0;
}

void SendProc(SOCKET s, std::queue<Files>* q)
{
    FILE* fp;
    SOCKET sock = s;
    char buf[BUF_SIZE];

    while (1)
    {
        if (q->empty())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;
        }

        // 큐에서 하나를 꺼냄
        Files sendFile = q->front();
        q->pop();
        if (strcmp(sendFile.name, "exit") == 0 && sendFile.size == 0)
        {
            std::cout << "연결 종료" << std::endl;
            break;
        }

        // 업로드라는것을 알림
        int type = 1;
        send(sock, (char*)&type, sizeof(int), 0);
        std::cout << "업로드 요청" << std::endl;

        // 파일 크기 보냄
        send(sock, (char*)&sendFile.size, sizeof(int), 0);

        // 파일명을 보냄
        send(sock, sendFile.name, MAX_MSG_LEN, 0);

        // 데이터 통신에 사용할 변수
        int sendSize = 0;
        fp = fopen(sendFile.name, "rb");

        // 파일 전송
        while (1)
        {
            sendSize = fread(buf, 1, BUF_SIZE, fp);
            if (sendSize < BUF_SIZE)
            {
                send(sock, buf, sendSize, 0);
                break;
            }
            send(sock, buf, BUF_SIZE, 0);
        }
        fclose(fp);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void RecvProc(SOCKET s, std::queue<Files>* q)
{

}