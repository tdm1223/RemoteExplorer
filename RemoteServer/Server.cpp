#include "Server.h"

Server::Server()
{
    numOfClient = 0;
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        exit(1);
    }
    SOCKET sock = SetServer(); // 대기 소켓 설정
    if (sock == -1)
    {
        perror("대기 소켓 오류");
    }
    else
    {
        std::cout << "Create Main Thread..." << std::endl;
        EventLoop(sock);
    }
}

Server::~Server()
{
    WSACleanup();
}

SOCKET Server::SetServer()
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == -1)
    {
        return -1;
    }

    // 서버 정보 설정
    SOCKADDR_IN servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // 소켓 주소와 네트워크 인터페이스 결합
    if (bind(sock, (sockaddr*)&servaddr, sizeof(servaddr)) == -1)
    {
        return -1;
    }

    //백 로그 큐 설정
    if (listen(sock, SOMAXCONN) == -1)
    {
        return -1;
    }
    return sock;
}

void Server::EventLoop(SOCKET sock)
{
    AddEvent(sock, FD_ACCEPT | FD_CLOSE);
    while (true)
    {
        // 이벤트 발생을 기다리면서 가장 처음 발생한 index를 반환
        int index = WSAWaitForMultipleEvents(numOfClient, eventArray, false, INFINITE, false);
        WSANETWORKEVENTS net_events;

        // 이벤트 종류 알아내기
        WSAEnumNetworkEvents(socketArray[index], eventArray[index], &net_events);
        if (net_events.lNetworkEvents == FD_ACCEPT)
        {
            SOCKADDR_IN clientAddress = { 0 };
            int len = sizeof(clientAddress);
            SOCKET sock = accept(socketArray[0], (SOCKADDR*)&clientAddress, &len);

            if (numOfClient == FD_SETSIZE)
            {
                closesocket(sock);
            }
            AddEvent(sock, FD_READ | FD_CLOSE);
            std::cout << "[" << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "] 연결 성공" << std::endl;
        }
        else if (net_events.lNetworkEvents == FD_READ)
        {
            // RECV용 버퍼 선언 및 초기화
            char recvBuffer[BUF_SIZE];
            memset(recvBuffer, 0, BUF_SIZE);

            // 버퍼 사이즈 만큼의 데이터를 가져와서 RECV용 버퍼에 저장
            int byteLen = recv(socketArray[index], recvBuffer, BUF_SIZE, 0);
            std::cout << "Server receive " << byteLen << "bytes from " << index << std::endl;
            if (byteLen > 0)
            {
                Parser parser;
                Packet result;
                parser.Parsing(recvBuffer, byteLen, result);

                std::cout << "클라로 부터 받은 메시지의 타입 : " << result.command << std::endl;
                
                if (result.command == UPLOAD)
                {
                    std::string fileName(result.buf.begin(), result.buf.end());

                    Packet fileSizePacket;
                    memset(recvBuffer, 0, sizeof(recvBuffer));

                    int s = 0;
                    while(1)
                    {
                        byteLen = recv(socketArray[index], recvBuffer, BUF_SIZE, 0);
                        if(byteLen == -1)
                        {
                            continue;
                        }
                        break;
                    }

                    parser.Parsing(recvBuffer, byteLen, fileSizePacket);
                    std::cout << "파일크기 사이즈 : " << fileSizePacket.size << std::endl;
                    std::string size(fileSizePacket.buf.begin(), fileSizePacket.buf.end());
                    int fileSize = atoi(size.c_str());
                    std::cout << "파일이름 : " << fileName << " 파일 크기 : " << fileSize << std::endl;

                    // 파일 받음
                    std::cout << "Recv Proc Start" << std::endl;

                    // 파일 받는 로직
                    FILE* fp = fopen(fileName.c_str(), "wb+");
                    std::cout << "파일명 : " << fileName << std::endl;

                    memset(recvBuffer, 0, sizeof(recvBuffer));
                    byteLen = 0;
                    int totalSize = 0;
                    while ((byteLen = recv(socketArray[index], recvBuffer, BUF_SIZE, 0)) != 0)
                    {
                        Packet filePacket;
                        parser.Parsing(recvBuffer, byteLen, filePacket);
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
                        totalSize += (byteLen - filePacket.GetHeaderSize());
                        //std::cout << "받은 크기 : " << filePacket.GetHeaderSize() << " 전체 크기 : " << totalSize << std::endl;
                        fwrite(&filePacket.buf[0], 1, (byteLen - 2 * sizeof(int) - sizeof(char)), fp);
                    }
                    fclose(fp);
                    std::cout << "전송 완료" << std::endl;
                }
                else if (result.command == DOWNLOAD)
                {
                }
            }
        }
        else if (net_events.lNetworkEvents == FD_CLOSE)
        {
            std::thread closeThread([=] {CloseProc(index); });
            closeThread.join();
        }
    }
    closesocket(sock);
}

void Server::AddEvent(SOCKET sock, long eventType)
{
    HANDLE wsaEvent = WSACreateEvent();
    socketArray[numOfClient] = sock;
    eventArray[numOfClient] = wsaEvent;
    numOfClient++;
    WSAEventSelect(sock, wsaEvent, eventType);
}

void Server::GetClientAddress(SOCKADDR_IN& clientAddress, int index)
{
    int len = sizeof(clientAddress);
    getpeername(socketArray[index], (SOCKADDR*)&clientAddress, &len);
}

void Server::CloseProc(int index)
{
    SOCKADDR_IN clientAddress = { 0 };
    GetClientAddress(clientAddress, index);
    std::cout << "[" << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "] 연결 종료" << std::endl;

    closesocket(socketArray[index]);
    WSACloseEvent(eventArray[index]);

    numOfClient--;
    socketArray[index] = socketArray[numOfClient];
    eventArray[index] = eventArray[numOfClient];
}

void Server::RecvProc(int index)
{
    std::cout << "Recv Proc Start" << std::endl;
    SOCKADDR_IN clientAddress = { 0 };
    GetClientAddress(clientAddress, index);

    char buf[BUF_SIZE];
    recv(socketArray[index], buf, BUF_SIZE, 0);

    // 파일 크기 수신
    int fileSize = 0;
    memcpy(&fileSize, (int*)buf, sizeof(fileSize));

    // 파일명 수신
    char name[MAX_MSG_LEN];
    memcpy(name, buf + sizeof(fileSize), sizeof(name));

    // 파일 정보 출력
    std::cout << "[" << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "] 전송받는 파일 : " << name << " 전송받는 파일 크기 : " << fileSize << "Byte" << std::endl;

    // 파일 받는 로직
    FILE* fp = fopen(name, "wb+");
    if (fp == NULL)
    {
        std::cout << "파일 쓰기 오류. 해당 파일 생략" << std::endl;
        return;
    }

    memset(buf, 0, sizeof(buf));
    int readSize = 0;
    int totalSize = 0;
    while ((readSize = recv(socketArray[index], buf, BUF_SIZE, 0)) != 0)
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
    }
    fclose(fp);
}

void Server::SendProc(int index)
{
    int cnt = 0;
    std::vector<std::string> fileVector;

    std::string files;
    for (const fs::directory_entry& entry : fs::directory_iterator(fs::current_path()))
    {
        files = entry.path().string();
        size_t pos = files.rfind("\\");
        files = files.substr(pos + 1);
        fileVector.push_back(files);
        cnt++;
    }

    // 파일 개수를 보냄
    send(socketArray[index], (char*)&cnt, sizeof(int), 0);

    // 개수만큼 파일명을 보냄
    char sendBuf[MAX_MSG_LEN];
    for (auto tmp = fileVector.begin(); tmp != fileVector.end(); tmp++)
    {
        strcpy(sendBuf, tmp->c_str());
        send(socketArray[index], sendBuf, MAX_MSG_LEN, 0);
    }

    // 입력 대기
    while (1)
    {
        if (recv(socketArray[index], sendBuf, MAX_MSG_LEN, 0) != SOCKET_ERROR)
        {
            break;
        }
    }
    std::string fileName = sendBuf;

    std::cout << "다운로드 요청 파일 : " << fileName << std::endl;

    // 파일 크기 전송
    FILE* fp;
    int fileSize = 0;
    fp = fopen(fileName.c_str(), "rb");
    if (fp == NULL)
    {
        return;
    }
    // 파일 끝으로 위치 옮김
    fseek(fp, 0L, SEEK_END);

    // 파일 크기 얻음
    fileSize = ftell(fp);

    // 다시 파일 처음으로 위치 옮김
    fseek(fp, 0L, SEEK_SET);
    fclose(fp);
    send(socketArray[index], (char*)&fileSize, sizeof(fileSize), 0);

    // 파일 전송
    int sendSize = 0;
    int totalSize = 0;
    fp = fopen(fileName.c_str(), "rb");
    char buf[BUF_SIZE];
    cnt = 0;
    while (1)
    {
        sendSize = fread(buf, 1, BUF_SIZE, fp);
        totalSize += sendSize;
        cnt++;
        std::cout << "total : " << totalSize << std::endl;
        if (sendSize < BUF_SIZE && totalSize == fileSize)
        {
            send(socketArray[index], buf, sendSize, 0);
            break;
        }
        send(socketArray[index], buf, BUF_SIZE, 0);
    }
    fclose(fp);
    std::cout << "파일 전송 완료" << std::endl;
}
