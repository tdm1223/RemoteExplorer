//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <winsock2.h>
//
//#define BUF_SIZE 1024
//
//#pragma warning(disable:4996)
//#pragma comment(lib, "Ws2_32.lib")
//
//void ErrorHandling(char* message);
//
//// 데이터 수신 함수
//int ReceiveData(SOCKET s, char* buf, int len, int flags)
//{
//    // 통신에 필요한 변수 선언
//    int received;
//    char* ptr = buf;
//    int left = len;
//    // 남아있는 데이터가 있는경우 반복 수행
//    while (left > 0)
//    {
//        received = recv(s, ptr, left, flags);
//        // 더이상 데이터를 받지 못하는 경우 종료
//        if (received == 0)
//        {
//            break;
//        }
//        // 받아온 데이터가 존재한다면
//        // 받은만큼 left 감소
//        // 반은만큼 ptr 증가
//        left -= received;
//        ptr += received;
//    }
//    // 더 받을 수 있는 데이터의 길이를 반환
//    return (len - left);
//}
//
//int main()
//{
//    WSADATA wsaData;
//    SOCKET serverSocket, clientSocket;
//    SOCKADDR_IN servAdr, clntAdr;
//    TIMEVAL timeout;
//    fd_set reads, cpyReads;
//
//    int adrSz;
//    int strLen, fdNum, i;
//    char buf[BUF_SIZE];
//
//    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
//    {
//        ErrorHandling("WSAStartup() error!");
//    }
//
//    serverSocket = socket(PF_INET, SOCK_STREAM, 0);
//    memset(&servAdr, 0, sizeof(servAdr));
//    servAdr.sin_family = AF_INET;
//    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
//    servAdr.sin_port = htons(9000);
//
//    if (bind(serverSocket, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR)
//    {
//        ErrorHandling("bind() error");
//    }
//    if (listen(serverSocket, 5) == SOCKET_ERROR)
//    {
//        ErrorHandling("listen() error");
//    }
//
//    FD_ZERO(&reads);
//    FD_SET(serverSocket, &reads);
//
//    while (1)
//    {
//        cpyReads = reads;
//        timeout.tv_sec = 5;
//        timeout.tv_usec = 5000;
//
//        if ((fdNum = select(0, &cpyReads, 0, 0, &timeout)) == SOCKET_ERROR)
//        {
//            break;
//        }
//
//        if (fdNum == 0)
//        {
//            continue;
//        }
//
//        for (i = 0; i < reads.fd_count; i++)
//        {
//            if (FD_ISSET(reads.fd_array[i], &cpyReads))
//            {
//                // 연결 요청
//                if (reads.fd_array[i] == serverSocket)
//                {
//                    adrSz = sizeof(clntAdr);
//                    clientSocket = accept(serverSocket, (SOCKADDR*)&clntAdr, &adrSz);
//                    FD_SET(clientSocket, &reads);
//                    printf("클라 접속 : IP = %s, Port = %d\n", inet_ntoa(clntAdr.sin_addr), ntohs(clntAdr.sin_port));
//                }
//                else
//                {
//                    strLen = recv(reads.fd_array[i], buf, BUF_SIZE, 0);
//                    // 종료 요청
//                    if (strLen == 0)
//                    {
//                        FD_CLR(reads.fd_array[i], &reads);
//                        closesocket(cpyReads.fd_array[i]);
//                        printf("-> 클라이언트 종료 : IP : %s, Port = %d, desc = %d\n\n", inet_ntoa(clntAdr.sin_addr), ntohs(clntAdr.sin_port), cpyReads.fd_array[i]);
//                    }
//                    else
//                    {
//                        // 파일 이름 받기
//                        char fileName[256];
//                        sprintf(fileName, buf);
//                        printf("-> 받을 파일 이름:%s\n", fileName);
//
//                        // 파일 크기 받기
//                        int totalBytes;
//                        strLen = recv(reads.fd_array[i], (char*)&totalBytes, sizeof(totalBytes), 0);
//                        printf("-> 받을 파일 크기:%d\n", totalBytes);
//
//                        // 파일 열고 받기
//                        FILE* fp = fopen(fileName, "wb");
//                        int total = 0;
//                        while (1)
//                        {
//                            strLen = ReceiveData(reads.fd_array[i], buf, BUF_SIZE, 0);
//                            // 더 받을 데이터가 없음
//                            if (strLen == 0)
//                            {
//                                break;
//                            }
//                            else
//                            {
//                                fwrite(buf, 1, strLen, fp);
//                                total += strLen;
//                            }
//                        }
//                        fclose(fp);
//
//                        if (total == totalBytes)
//                        {
//                            printf("-> 파일을 성공적으로 받았습니다.\n");
//                        }
//                        else
//                        {
//                            printf("-> 파일을 제대로 받지 못했습니다.\n");
//                        }
//                    }
//                }
//            }
//        }
//    }
//    closesocket(serverSocket);
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