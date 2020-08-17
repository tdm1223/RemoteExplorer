#include<stdio.h>
#include<string.h>
#include<winsock2.h>

#define BUF_SIZE 100
#pragma comment(lib,"ws2_32")

void ErrorHandling(char* msg);

int main()
{
    WSADATA wsaData;
    SOCKET serverSock, clientSock;
    SOCKADDR_IN serverAddress, clientAddress;

    SOCKET socketArray[WSA_MAXIMUM_WAIT_EVENTS]; // 소켓의 핸들 정보를 저장하기 위한 배열
    WSAEVENT eventArray[WSA_MAXIMUM_WAIT_EVENTS]; // 이벤트 오브젝트 핸들 정보를 저장하기 위한 배열
    WSANETWORKEVENTS netEvents;

    int numOfClientSock = 0;
    int strLen;
    int posInfo, startIdx;
    char msg[BUF_SIZE];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        ErrorHandling("WSAStartup() error!");
    }

    // 서버 소켓 생성 후 초기화
    serverSock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(9000);

    // bind
    if (bind(serverSock, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        ErrorHandling("bind() error");
    }

    // listen
    if (listen(serverSock, 5) == SOCKET_ERROR)
    {
        ErrorHandling("listen() error");
    }

    WSAEVENT newEvent = WSACreateEvent(); // 이벤트 오브젝트 생성

    // accept 요청, serverSock의 변화발생시 newEvent를 signaled상태로 바꾼다.
    if (WSAEventSelect(serverSock, newEvent, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
    {
        ErrorHandling("WSAEventSelect() error");
    }

    // 동일한 인덱스에 정보 저장
    socketArray[numOfClientSock] = serverSock;
    eventArray[numOfClientSock] = newEvent;
    numOfClientSock++;

    while (1)
    {
        posInfo = WSAWaitForMultipleEvents(numOfClientSock, eventArray, FALSE, WSA_INFINITE, FALSE);
        startIdx = posInfo - WSA_WAIT_EVENT_0; // 실제로 변화가 발생한 이벤트 오브젝트의 첫번째 index 값

        for (int i = startIdx; i < numOfClientSock; i++)
        {
            int signaledEventIdx = WSAWaitForMultipleEvents(1, &eventArray[i], TRUE, 0, FALSE); // 확인을 위한 함수 호출
            if ((signaledEventIdx == WSA_WAIT_FAILED || signaledEventIdx == WSA_WAIT_TIMEOUT))
            {
                continue; // 반환 결과가 위에꺼면 CONTINUE
            }
            else
            {
                signaledEventIdx = i;
                WSAEnumNetworkEvents(socketArray[signaledEventIdx], eventArray[signaledEventIdx], &netEvents);

                if (netEvents.lNetworkEvents & FD_ACCEPT) // 연결 요청시
                {
                    if (netEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
                    {
                        puts("Accept Error");
                        break;
                    }
                    int clientAddressLength = sizeof(clientAddress);
                    clientSock = accept(socketArray[signaledEventIdx], (SOCKADDR*)&clientAddress, &clientAddressLength);
                    newEvent = WSACreateEvent();
                    WSAEventSelect(clientSock, newEvent, FD_READ | FD_CLOSE);

                    eventArray[numOfClientSock] = newEvent;
                    socketArray[numOfClientSock] = clientSock;
                    numOfClientSock++;
                    printf("클라 접속 : IP = %s, Port = %d\n", inet_ntoa(clientAddress.sin_addr), ntohs(clientAddress.sin_port));
                }

                if (netEvents.lNetworkEvents & FD_READ) // 데이터 수신
                {
                    if (netEvents.iErrorCode[FD_READ_BIT] != 0)
                    {
                        puts("Read Error");
                        break;
                    }
                    strLen = recv(socketArray[signaledEventIdx], msg, sizeof(msg), 0);
                    
                    // 파일 이름 받기
                    char fileName[256];
                    sprintf(fileName, msg);
                    printf("-> 받을 파일 이름:%s\n", msg);

                    send(socketArray[signaledEventIdx], msg, strLen, 0);
                }

                if (netEvents.lNetworkEvents & FD_CLOSE) // 종료 요청
                {
                    if (netEvents.iErrorCode[FD_CLOSE_BIT] != 0)
                    {
                        puts("Close Error");
                        break;
                    }
                    WSACloseEvent(eventArray[signaledEventIdx]);
                    closesocket(socketArray[signaledEventIdx]);
                    printf("종료 된 소켓의 핸들 %d\n", socketArray[signaledEventIdx]);
                    numOfClientSock--;
                    for (int i = signaledEventIdx; i < numOfClientSock; i++)
                    {
                        socketArray[i] = socketArray[i + 1];
                        eventArray[i] = eventArray[i + 1];
                    }
                }
            }
        }
    }
    WSACleanup();
    return 0;
}

void ErrorHandling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}