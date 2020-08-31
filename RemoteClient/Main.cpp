#include<winsock2.h>
#include<stdlib.h>
#include<stdio.h>
#include<thread>
#include<string>
#include<iostream>
#include<queue>

#pragma comment(lib, "ws2_32")

#define PORT 9000
#define MAX_MSG_LEN 256
#define SERVER_IP "127.0.0.1"
#define BUF_SIZE 4096
#define MESSAGE_SIZE 20

void SendProc(SOCKET sock);

//파일 기본 정보
struct Files
{
    char name[MAX_MSG_LEN];
    unsigned int size;
    int index = 0;
};

int main()
{
    // 윈속 초기화
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        std::cout << "윈속 초기화 에러" << std::endl;
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

    std::thread sendThread(SendProc, sock);
    sendThread.join();

    WSACleanup(); // 윈속 해제화
    return 0;
}

void SendProc(SOCKET s)
{
    SOCKET sock = s;
    FILE* fp;
    Files sendFile;
    std::string fileName;
    char endMessage[MESSAGE_SIZE];
    char buf[BUF_SIZE];
    char name[256];

    while (true)
    {
        fileName.clear();
        std::cin >> fileName;
        strcpy(sendFile.name, fileName.c_str());

        // 종료라면 소켓 닫기
        if (strcmp(sendFile.name, "exit") == 0)
        {
            closesocket(sock);
            break;
        }

        if (strcmp(sendFile.name, "test") == 0)
        {
            for (int i = 0; i < 5; i++)
            {
                std::string name = std::to_string(i);
                name += ".txt";
                std::cout << "name : " << name << std::endl;
                ZeroMemory(sendFile.name, MAX_MSG_LEN);
                strcpy(sendFile.name, name.c_str());

                fp = fopen(sendFile.name, "rb");
                // 파일 끝으로 위치 옮김
                fseek(fp, 0L, SEEK_END);

                // 파일 크기 얻음
                sendFile.size = ftell(fp);

                // 다시 파일 처음으로 위치 옮김
                fseek(fp, 0L, SEEK_SET);

                // 파일 기본 정보 전송
                std::cout << "전송하는 파일명 : " << sendFile.name << " 전송하는 파일 크기 : " << sendFile.size << " Byte" << std::endl;
                send(sock, (char*)&sendFile, sizeof(sendFile), 0);

                recv(sock, endMessage, MESSAGE_SIZE, 0);
                if (strcmp(endMessage, "start") == 0)
                {
                    std::cout << "서버로 부터 파일 보내도 된다는 메시지를 받음" << std::endl;
                }

                // 데이터 통신에 사용할 변수
                int retval;
                int numread = 0;
                int numtotal = 0;

                // 파일 전송
                while (1)
                {
                    numread = fread(buf, 1, BUF_SIZE, fp);

                    if (numread > 0)
                    {
                        send(sock, buf, numread, 0);
                        numtotal += numread;
                    }
                    else if (numread == 0 && numtotal == sendFile.size)
                    {
                        std::cout << "파일 전송 완료" << std::endl;
                        break;
                    }
                }

                recv(sock, endMessage, MESSAGE_SIZE, 0);
                if (strcmp(endMessage, "end") == 0)
                {
                    std::cout << "파일 전송 완료됨을 받음" << std::endl;
                }
                fclose(fp);
            }
        }

        if (strcmp(sendFile.name, "test2") == 0)
        {
            for (int i = 5; i < 10; i++)
            {
                std::string name = std::to_string(i);
                name += ".txt";
                std::cout << "name : " << name << std::endl;
                ZeroMemory(sendFile.name, MAX_MSG_LEN);
                strcpy(sendFile.name, name.c_str());

                fp = fopen(sendFile.name, "rb");
                // 파일 끝으로 위치 옮김
                fseek(fp, 0L, SEEK_END);

                // 파일 크기 얻음
                sendFile.size = ftell(fp);

                // 다시 파일 처음으로 위치 옮김
                fseek(fp, 0L, SEEK_SET);

                // 파일 기본 정보 전송
                std::cout << "전송하는 파일명 : " << sendFile.name << " 전송하는 파일 크기 : " << sendFile.size << " Byte" << std::endl;
                send(sock, (char*)&sendFile, sizeof(sendFile), 0);

                recv(sock, endMessage, MESSAGE_SIZE, 0);
                if (strcmp(endMessage, "start") == 0)
                {
                    std::cout << "서버로 부터 파일 보내도 된다는 메시지를 받음" << std::endl;
                }

                // 데이터 통신에 사용할 변수
                int retval;
                int numread = 0;
                int numtotal = 0;

                // 파일 전송
                while (1)
                {
                    numread = fread(buf, 1, BUF_SIZE, fp);

                    if (numread > 0)
                    {
                        send(sock, buf, numread, 0);
                        numtotal += numread;
                    }
                    else if (numread == 0 && numtotal == sendFile.size)
                    {
                        std::cout << "파일 전송 완료" << std::endl;
                        break;
                    }
                }

                recv(sock, endMessage, MESSAGE_SIZE, 0);
                if (strcmp(endMessage, "end") == 0)
                {
                    std::cout << "파일 전송 완료됨을 받음" << std::endl;
                }
                fclose(fp);
            }
        }

        fp = fopen(sendFile.name, "rb");
        if (fp == NULL)
        {
            std::cout << "파일이 없습니다. 파일명을 확인하세요" << std::endl;
            continue;
        };

        // 파일 끝으로 위치 옮김
        fseek(fp, 0L, SEEK_END);

        // 파일 크기 얻음
        sendFile.size = ftell(fp);

        // 다시 파일 처음으로 위치 옮김
        fseek(fp, 0L, SEEK_SET);

        // 파일 기본 정보 전송
        std::cout << "전송하는 파일명 : " << sendFile.name << " 전송하는 파일 크기 : " << sendFile.size << " Byte" << std::endl;
        send(sock, (char*)&sendFile, sizeof(sendFile), 0);

        // 서버로부터 파일을 보내도 된다는 메시지를 받을때까지 대기
        recv(sock, endMessage, MESSAGE_SIZE, 0);
        //if (strcmp(endMessage, "start") == 0)
        //{
        //    std::cout << "서버로 부터 파일 보내도 된다는 메시지를 받음" << std::endl;
        //}

        // 데이터 통신에 사용할 변수
        int retval;
        int numread = 0;
        int numtotal = 0;

        // 파일 전송
        while (1)
        {
            numread = fread(buf, 1, BUF_SIZE, fp);

            if (numread > 0)
            {
                send(sock, buf, numread, 0);
                numtotal += numread;
            }
            else if (numread == 0 && numtotal == sendFile.size)
            {
                std::cout << "파일 전송 완료" << std::endl;
                break;
            }
        }
        fclose(fp);

        recv(sock, endMessage, MESSAGE_SIZE, 0);
        if (strcmp(endMessage, "end") == 0)
        {
            std::cout << "파일 전송 완료됨을 받음" << std::endl;
        }
    }
}