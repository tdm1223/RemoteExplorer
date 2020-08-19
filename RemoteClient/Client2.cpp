//#pragma comment(lib, "ws2_32")
//#include <winsock2.h>
//#include <string.h>
//#include <stdlib.h>
//#include <stdio.h>
//
//#define SERVERIP   "127.0.0.1"
//#define SERVERPORT 9000
//#define BUF_SIZE    8192
//
//// 사용자 정의 데이터 수신 함수
//int recvn(SOCKET, char*, int, int);
//
////파일 기본 정보
//struct Files
//{
//    char name[255];
//    unsigned int byte;
//};
//
//int main()
//{
//    //(파일 크기 / 버퍼 사이즈) 계산한 값을 while문으로 돌리기 위한 변수
//    unsigned int count;
//
//    //파일 이름 및 크기 확인
//    FILE* fp;
//    Files files;
//
//    do
//    {
//        printf("보낼 파일을 적어주세요 : ");
//        scanf("%s", files.name);
//        getchar(); //엔터 제거용 함수
//    } while (files.name == NULL);
//
//    fp = fopen(files.name, "rb");
//    if (fp == NULL)
//    {
//        printf("FILE Pointer ERROR");
//        exit(1);
//    };
//
//    //파일 끝으로 위치 옮김
//    fseek(fp, 0L, SEEK_END);
//
//    //파일 바이트값 출력
//    files.byte = ftell(fp);
//
//    //다시 파일 처음으로 위치 옮김
//    fseek(fp, 0L, SEEK_SET);
//
//    //send, recv 함수 출력값 저장용
//    int retval;
//
//    // 윈속 초기화
//    WSADATA wsa;
//    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
//    {
//        return 1;
//    }
//
//    // socket()
//    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
//    if (sock == INVALID_SOCKET)
//    {
//        exit(1);
//    }
//
//    // connect()
//    SOCKADDR_IN serveraddr; //서버와 통신용 소켓
//    ZeroMemory(&serveraddr, sizeof(serveraddr));
//    serveraddr.sin_family = AF_INET;
//    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
//    serveraddr.sin_port = htons(SERVERPORT);
//    retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
//    if (retval == SOCKET_ERROR)
//    {
//        exit(1);
//    }
//
//    // 데이터 통신에 사용할 변수
//    char buf[BUF_SIZE]; //보낼 데이터를 저장할 공간
//
//    //파일 기본 정보 전송
//    retval = send(sock, (char*)&files, sizeof(files), 0);
//    if (retval == SOCKET_ERROR)
//    {
//        exit(1);
//    }
//
//    unsigned int per;
//    per = count = files.byte / BUF_SIZE;
//    while (count)
//    {
//        system("cls");
//        printf("전송하는 파일 : %s, 전송하는 파일 크기 : %d Byte\n", files.name, files.byte);
//
//        //파일 읽어서 버퍼에 저장
//        fread(buf, 1, BUF_SIZE, fp);
//
//        //전송
//        retval = send(sock, buf, BUF_SIZE, 0);
//        if (retval == SOCKET_ERROR)
//        {
//            exit(1);
//        }
//
//        printf("\n진행도 : %d %%", (per - count) * 100 / per);
//        Sleep(100);
//        count--;
//    }
//
//    //남은 파일 크기만큼 나머지 전송
//    count = files.byte - (per * BUF_SIZE);
//    fread(buf, 1, count, fp);
//
//    retval = send(sock, buf, BUF_SIZE, 0);
//    if (retval == SOCKET_ERROR)
//    {
//        exit(1);
//    }
//
//    system("cls");
//    printf("전송하는 파일 : %s, 전송하는 파일 크기 : %d Byte\n", files.name, files.byte);
//    printf("\n진행도 : 100 %%");
//    printf("\n\n전송이 완료되었습니다.");
//
//    // closesocket()
//    closesocket(sock);
//
//    // 윈속 종료
//    WSACleanup();
//
//    //파일포인터 닫기
//    fclose(fp);
//
//    return 0;
//}
//
//int recvn(SOCKET s, char* buf, int len, int flags)
//{
//    int received;
//    char* ptr = buf;
//    int left = len;
//
//    while (left > 0)
//    {
//        received = recv(s, ptr, left, flags);
//        if (received == SOCKET_ERROR)
//        {
//            return SOCKET_ERROR;
//        }
//        else if (received == 0)
//        {
//            break;
//        }
//        left -= received;
//        ptr += received;
//    }
//
//    return (len - left);
//}
//
