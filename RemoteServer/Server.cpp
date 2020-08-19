//#include<stdio.h>
//#include<string.h>
//#include<winsock2.h>
//
//#define BUF_SIZE 8192
//#pragma comment(lib,"ws2_32")
//
//void ErrorHandling(char* msg);
//int recvn(SOCKET, char*, int, int);
//
////파일 기본 정보
//struct Files {
//    char name[255];
//    unsigned int byte;
//};
//
//int main()
//{
//    //send, recv 함수 출력값 저장용
//    int retval;
//
//    //(파일 크기 / 버퍼 사이즈) 계산한 값을 while문으로 돌리기 위한 변수
//    unsigned int count;
//
//    WSADATA wsaData;
//    SOCKET serverSock, clientSock;
//    SOCKADDR_IN serverAddress, clientAddress;
//
//    SOCKET socketArray[WSA_MAXIMUM_WAIT_EVENTS]; // 소켓의 핸들 정보를 저장하기 위한 배열
//    WSAEVENT eventArray[WSA_MAXIMUM_WAIT_EVENTS]; // 이벤트 오브젝트 핸들 정보를 저장하기 위한 배열
//    WSANETWORKEVENTS netEvents;
//
//    int numOfClientSock = 0;
//    int strLen;
//    int posInfo, startIdx;
//    char msg[BUF_SIZE];
//
//    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
//    {
//        ErrorHandling("WSAStartup() error!");
//    }
//
//    // 서버 소켓 생성 후 초기화
//    serverSock = socket(PF_INET, SOCK_STREAM, 0);
//    memset(&serverAddress, 0, sizeof(serverAddress));
//    serverAddress.sin_family = AF_INET;
//    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
//    serverAddress.sin_port = htons(9000);
//
//    // bind
//    if (bind(serverSock, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
//    {
//        ErrorHandling("bind() error");
//    }
//
//    // listen
//    if (listen(serverSock, SOMAXCONN) == SOCKET_ERROR)
//    {
//        ErrorHandling("listen() error");
//    }
//
//    WSAEVENT newEvent = WSACreateEvent(); // 이벤트 오브젝트 생성
//
//    // accept 요청, serverSock의 변화발생시 newEvent를 signaled상태로 바꾼다.
//    if (WSAEventSelect(serverSock, newEvent, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
//    {
//        ErrorHandling("WSAEventSelect() error");
//    }
//
//    // 동일한 인덱스에 정보 저장
//    socketArray[numOfClientSock] = serverSock;
//    eventArray[numOfClientSock] = newEvent;
//    numOfClientSock++;
//
//    while (1)
//    {
//        posInfo = WSAWaitForMultipleEvents(numOfClientSock, eventArray, FALSE, WSA_INFINITE, FALSE);
//        startIdx = posInfo - WSA_WAIT_EVENT_0; // 실제로 변화가 발생한 이벤트 오브젝트의 첫번째 index 값
//
//        for (int i = startIdx; i < numOfClientSock; i++)
//        {
//            int signaledEventIdx = WSAWaitForMultipleEvents(1, &eventArray[i], TRUE, 0, FALSE); // 확인을 위한 함수 호출
//            if ((signaledEventIdx == WSA_WAIT_FAILED || signaledEventIdx == WSA_WAIT_TIMEOUT))
//            {
//                continue; // 반환 결과가 위에꺼면 CONTINUE
//            }
//            else
//            {
//                signaledEventIdx = i;
//                WSAEnumNetworkEvents(socketArray[signaledEventIdx], eventArray[signaledEventIdx], &netEvents);
//
//                if (netEvents.lNetworkEvents & FD_ACCEPT) // 연결 요청시
//                {
//                    if (netEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
//                    {
//                        puts("Accept Error");
//                        break;
//                    }
//                    int clientAddressLength = sizeof(clientAddress);
//                    clientSock = accept(socketArray[signaledEventIdx], (SOCKADDR*)&clientAddress, &clientAddressLength);
//                    newEvent = WSACreateEvent();
//                    WSAEventSelect(clientSock, newEvent, FD_READ | FD_CLOSE);
//
//                    eventArray[numOfClientSock] = newEvent;
//                    socketArray[numOfClientSock] = clientSock;
//                    printf("%d 클라 접속 : IP = %s, Port = %d\n", numOfClientSock, inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
//                    numOfClientSock++;
//                }
//
//                if (netEvents.lNetworkEvents & FD_READ) // 데이터 수신
//                {
//                    if (netEvents.iErrorCode[FD_READ_BIT] != 0)
//                    {
//                        puts("Read Error");
//                        break;
//                    }
//
//                    //클라이언트로 부터 파일 기본 정보 받기
//                    FILE* fp = NULL;
//                    Files files;
//
//                    retval = recvn(socketArray[signaledEventIdx], (char*)&files, sizeof(files), 0);
//                    if (retval == SOCKET_ERROR) {
//                        exit(1);
//                    }
//                    //기존 파일 여부 확인
//                    fp = fopen(files.name, "rb");
//                    if (fp == NULL) { printf("같은 파일 이름이 없으므로 전송을 진행합니다.\n"); }
//                    else
//                    {
//                        system("cls");
//                        printf("이미 같은 이름의 파일이 존재 합니다.\n전송을 종료합니다.\n");
//                        fclose(fp);
//                        break;
//                    }
//
//                    printf("%d 클라로부터 파일을 전송받습니다.\n", signaledEventIdx);
//                    printf("전송받는 파일 : %s, 전송받는 파일 크기 : %d Byte\n", files.name, files.byte);
//                    printf("\n클라이언트로 부터 파일을 전송 받는 중 입니다.\n");
//
//                    fp = fopen(files.name, "wb");
//
//                    count = files.byte / BUF_SIZE;
//                    printf("count : %d\n", count);
//
//                    while (count)
//                    {
//                        //받기
//                        retval = recvn(socketArray[signaledEventIdx], msg, BUF_SIZE, 0);
//                        printf("retval : %d", retval);
//                        while (1);
//                        if (retval == SOCKET_ERROR)
//                        {
//                            exit(1);
//                        }
//
//                        //파일 작성 작업
//                        fwrite(msg, 1, BUF_SIZE, fp);
//                        count--;
//                    }
//
//                    //남은 파일 크기만큼 나머지 받기
//                    count = files.byte - ((files.byte / BUF_SIZE) * BUF_SIZE);
//                    retval = recvn(socketArray[signaledEventIdx], msg, BUF_SIZE, 0);
//                    if (retval == SOCKET_ERROR)
//                    {
//                        exit(1);
//                    }
//
//                    fwrite(msg, 1, count, fp);
//
//                    //파일포인터 닫기
//                    fclose(fp);
//                    printf("\n파일 전송이 완료되었습니다.\n");
//                }
//
//                if (netEvents.lNetworkEvents & FD_CLOSE) // 종료 요청
//                {
//                    if (netEvents.iErrorCode[FD_CLOSE_BIT] != 0)
//                    {
//                        puts("Close Error");
//                        break;
//                    }
//                    WSACloseEvent(eventArray[signaledEventIdx]);
//                    closesocket(socketArray[signaledEventIdx]);
//                    printf("%d 클라 연결 종료\n", signaledEventIdx);
//                    numOfClientSock--;
//                    for (int i = signaledEventIdx; i < numOfClientSock; i++)
//                    {
//                        socketArray[i] = socketArray[i + 1];
//                        eventArray[i] = eventArray[i + 1];
//                    }
//                }
//            }
//        }
//    }
//    WSACleanup();
//    return 0;
//}
//
//void ErrorHandling(char* msg)
//{
//    fputs(msg, stderr);
//    fputc('\n', stderr);
//    exit(1);
//}
//
//int recvn(SOCKET s, char* buf, int len, int flags)
//{
//    SOCKET sock = (SOCKET)s;
//    int received;
//    char* ptr = buf;
//    int left = len;
//
//    while (left > 0)
//    {
//        received = recv(sock, ptr, left, flags);
//        printf("received : %d\n", received);
//        if (received == SOCKET_ERROR)
//        {
//            printf("에러\n");
//            return SOCKET_ERROR;
//        }
//        else if (received == 0)
//        {
//            break;
//        }
//        left -= received;
//        ptr += received;
//    }
//    closesocket(sock);
//    return (len - left);
//}