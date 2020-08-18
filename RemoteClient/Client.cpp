//#include<stdio.h>
//#include<stdlib.h>
//#include<string.h>
//#include<winsock2.h>
//#include<process.h>
//
//#define BUF_SIZE 1024
//#pragma comment(lib,"ws2_32")
//
//void ErrorHandling(char* message);
//unsigned WINAPI Proc(void *arg); // 메시지 수신 쓰레드 실행 함수
//
//int main()
//{
//    WSADATA wsaData;
//    SOCKET clientSocket;
//    int strLen;
//    SOCKADDR_IN serverAddress;
//
//    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
//    {
//        ErrorHandling("WSAStartup() error!");
//    }
//
//    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
//    if (clientSocket == INVALID_SOCKET)
//    {
//        ErrorHandling("socket() error");
//    }
//
//    memset(&serverAddress, 0, sizeof(serverAddress));
//    serverAddress.sin_family = AF_INET;
//    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
//    serverAddress.sin_port = htons(9000);
//
//    if (connect(clientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
//    {
//        ErrorHandling("connect() error!");
//    }
//    else
//    {
//        printf("서버에 접속 성공 : IP = %s, Port = %d\n", inet_ntoa(serverAddress.sin_addr), ntohs(serverAddress.sin_port));
//    }
//
//    DWORD dwThreadID1;
//    HANDLE hThread1 = (HANDLE)_beginthreadex(NULL, 0, Proc, (void*)clientSocket, 0, (unsigned*)&dwThreadID1);
//
//    WaitForSingleObject(hThread1, INFINITE);
//
//    closesocket(clientSocket);
//    WSACleanup();
//    return 0;
//}
//
//void ErrorHandling(char* message)
//{
//    fputs(message, stderr);
//    fputc('\n', stderr);
//    exit(1);
//}
//
//unsigned WINAPI Proc(void *arg)
//{
//    char message[BUF_SIZE];
//    SOCKET sock = (SOCKET)arg;
//    while (1)
//    {
//        fputs("Input message(Q to quit): ", stdout);
//        fgets(message, BUF_SIZE, stdin);
//        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
//        {
//            closesocket(sock);
//            break;
//        }
//
//        // 보낼 파일 설정
//        char sendFileName[256] = "test.txt";
//
//        // 파일 열고 파일명 보내기
//        //FILE* fp = fopen(sendFileName, "rb");
//        char fileName[256];
//        ZeroMemory(fileName, 256);
//        sprintf(fileName, sendFileName);
//        send(sock, fileName, strlen(fileName), 0);
//        printf("보내는 파일의 이름 : %s\n", fileName);
//
//        int strLen = recv(sock, message, BUF_SIZE - 1, 0);
//        message[strLen] = 0;
//        printf("Message from server: %s\n", message);
//    }
//}
