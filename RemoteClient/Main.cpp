#include<winsock2.h>
#include<stdlib.h>
#include<stdio.h>
#include<thread>
#include<string>
#include<iostream>
#include<queue>
#include<chrono>

#pragma comment(lib, "ws2_32")

#define PORT 9000
#define MAX_MSG_LEN 256
#define SERVER_IP "127.0.0.1"
#define BUF_SIZE 512
#define MESSAGE_SIZE 20

//파일 기본 정보
struct Files
{
    char name[MAX_MSG_LEN];
    int nameLen = 0;
    unsigned int size;
};

void SendProc(SOCKET s, std::queue<Files>* q);

int main()
{
    std::queue<Files> sendQueue;
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

    while (true)
    {
        fileName.clear();
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
    sendThread.join();
    closesocket(sock);
    WSACleanup(); // 윈속 해제화
    return 0;
}

void SendProc(SOCKET s, std::queue<Files>* q)
{
    FILE* fp;
    SOCKET sock = s;
    char endMessage[MESSAGE_SIZE];
    char buf[BUF_SIZE];
    char name[256];

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