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
            // 버퍼에서 헤더를 분석할 순서 인지를 판별하기위한 변수
            bool isHeaderPacket = false;

            // 보낸 데이터 길이
            unsigned int sendByteLen;

            // 받은 데이터, 보낸 데이터 바이트 길이를 받는 변수
            unsigned int byteLen;

            // 데이터를 처리할 수 있는 길이가 되었는지 확인용 변수
            // 현재 대기 버퍼안에 있는 데이터의 길이를 저장
            unsigned int curLen;

            // 버퍼의 인덱스 포인터
            char* recvBufferIndex = 0;

            // data 정리용 포인터
            char* dataSortingIndex = 0;

            // send buf 용 인덱스 포인터
            char* sendBufferOffset = 0;

            // RECV용 버퍼 선언
            char recvBuffer[BUF_SIZE];

            // SEND용 버퍼 선언 (헤더 + 메세지)
            char sendBuffer[BUF_SIZE];

            // message SEND용 버퍼 선언
            char bufSendMsg[BUF_SIZE];

            // 시스템용 대기 버퍼 선언
            char waitBuffer[BUF_SIZE];

            // 헤더 처리 버퍼
            char headerBuffer[BUF_SIZE];

            // 메세지 처리 버퍼
            char msgBuffer[BUF_SIZE];

            // 대기 버퍼 스왑용 버퍼
            char tmpBuffer[BUF_SIZE];

            // 메세지 헤더 구조체 포인터 선언
            Packet* recvHeaderPacket;

            // 송신용 메세지 헤더 구조체 선언
            Packet hdr;
            std::string Msg = "";

            // 대기버퍼에 있는 데이터 길이 초기
            curLen = 0;

            // 대기 버퍼 초기화
            memset(waitBuffer, 0, BUF_SIZE);

            // 버퍼 포인터 초기화 대기 버퍼의 시작 주소를 가진다.
            recvBufferIndex = waitBuffer;

            // data 수신 처리 receive 버퍼 초기화
            memset(recvBuffer, 0, BUF_SIZE);

            // data를 가져옴
            // 최대 버퍼 사이즈 만큼의 데이터를 가져와서 buf에 저장
            // recv 함수는 가져온 byte의 길이를 return
            byteLen = recv(socketArray[index], recvBuffer, BUF_SIZE, 0);
            std::cout << "Server receive " << byteLen << "bytes from " << index << std::endl;
            if (byteLen > BUF_SIZE)
            {
                break;
            }

            if (byteLen > 0)
            {
                // 길이만큼만 memcpy를 통해 저장
                memcpy(recvBufferIndex, recvBuffer, byteLen);
                curLen += byteLen;
                recvBufferIndex += byteLen;
                byteLen = 0;

                // 헤더 분석
                if (!isHeaderPacket)
                {
                    // 헤더 길이 이상의 data가 있는지 확인
                    if (curLen >= sizeof(recvHeaderPacket))
                    {
                        // 헤더 길이 이상 존재한다면 header buffer에 저장
                        memset(headerBuffer, 0, sizeof(headerBuffer));
                        memcpy(headerBuffer, waitBuffer, sizeof(recvHeaderPacket));
                        headerBuffer[sizeof(recvHeaderPacket)] = '\0';
                        recvHeaderPacket = (Packet*)headerBuffer;

                        // 버퍼의 data를 정리
                        dataSortingIndex = waitBuffer + sizeof(recvHeaderPacket);
                        memset(tmpBuffer, 0, sizeof(tmpBuffer));
                        memcpy(tmpBuffer, dataSortingIndex, sizeof(waitBuffer) - sizeof(recvHeaderPacket));

                        memset(waitBuffer, 0, sizeof(waitBuffer));
                        memcpy(waitBuffer, tmpBuffer, sizeof(tmpBuffer));

                        curLen -= sizeof(recvHeaderPacket);
                        recvBufferIndex -= sizeof(recvHeaderPacket);
                        isHeaderPacket = true;
                    }
                }

                if (isHeaderPacket)
                {
                    // 분석된 헤더에 존재했던 메시지 길이만큼의 데이터가 존재하는지 확인
                    if (curLen >= recvHeaderPacket->size)
                    {
                        // 데이터가 존재한다면 msgBuffer에 복사
                        memset(msgBuffer, 0, sizeof(msgBuffer));
                        memcpy(msgBuffer, waitBuffer, recvHeaderPacket->size);
                        msgBuffer[recvHeaderPacket->size] = '\0';

                        // 버퍼의 data를 정리
                        dataSortingIndex = waitBuffer + recvHeaderPacket->size;
                        memset(tmpBuffer, 0, sizeof(tmpBuffer));
                        memcpy(tmpBuffer, dataSortingIndex, sizeof(waitBuffer) - recvHeaderPacket->size);

                        memset(waitBuffer, 0, sizeof(waitBuffer));
                        memcpy(waitBuffer, tmpBuffer, sizeof(tmpBuffer));

                        curLen -= recvHeaderPacket->size;
                        recvBufferIndex -= recvHeaderPacket->size;
                        isHeaderPacket = false;

                        std::cout << "msgType : " << recvHeaderPacket->command << std::endl;
                        if (recvHeaderPacket->command == UPLOAD)
                        {
                            Msg = "received UPLOAD message";
                            std::cout << Msg << std::endl << std::endl;
                            strcpy(bufSendMsg, Msg.c_str());

                            memset(&hdr, 0, sizeof(hdr));
                            hdr.command = UPLOAD;
                            hdr.size = Msg.length();

                            sendBufferOffset = sendBuffer;
                            memcpy(sendBufferOffset, &hdr, sizeof(hdr));
                            sendBufferOffset = sendBufferOffset + sizeof(hdr);
                            memcpy(sendBufferOffset, bufSendMsg, hdr.size);

                            sendByteLen = send(socketArray[index], sendBuffer, sizeof(hdr) + hdr.size, 0);

                            std::string m = msgBuffer;
                            std::thread recvThread([=] {RecvProc(index); });
                            recvThread.join();
                        }
                        else if (recvHeaderPacket->command == DOWNLOAD)
                        {
                            std::cout << std::endl << "received DOWNLOAD message" << std::endl;
                            std::cout << "message length is " << recvHeaderPacket->size << "byte" << std::endl;
                            std::cout << "receive message : " << msgBuffer << std::endl;
                        }
                        else if (recvHeaderPacket->command == END)
                        {
                            std::cout << std::endl << "received END message" << std::endl;
                            std::cout << "message length is " << recvHeaderPacket->size << "byte" << std::endl;
                            std::cout << "receive message : " << msgBuffer << std::endl;
                        }
                    }
                }
            }
            /*Parser parser;
            Packet packet;

            recv(socketArray[index], buf, BUF_SIZE, 0);

            char* p = buf;
            int plen = BUF_SIZE;
            while (plen > 0)
            {
                int bytesRead = 0;
                if (parser.Parse(p, plen, bytesRead, packet))
                {
                    if (packet.command == UPLOAD)
                    {
                        std::cout << "업로드" << std::endl;
                    }
                    std::cout << "command : "<<packet.command << " size : " << packet.size << std::endl;
                    for (auto tmp : packet.data)
                    {
                        std::cout << tmp;
                    }
                    std::cout<<std::endl;
                }
                p += bytesRead;
                plen -= bytesRead;
            }
            int type = 0;*/

            //recv(socketArray[index], (char*)&type, sizeof(int), 0);
            //if (type == UPLOAD)
            //{
            //    std::thread recvThread([=] {RecvProc(index); });
            //    recvThread.join();
            //}
            //else if (type == DOWNLOAD)
            //{
            //    std::thread sendThread([=] {SendProc(index); });
            //    sendThread.join();
            //}
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

    //char buf[BUF_SIZE];
    //recv(socketArray[index], buf, BUF_SIZE, 0);

    //// 파일 크기 수신
    //int fileSize = 0;
    //memcpy(&fileSize, (int*)buf, sizeof(fileSize));

    //// 파일명 수신
    //char name[MAX_MSG_LEN];
    //memcpy(name, buf + sizeof(fileSize), sizeof(name));

    //// 파일 정보 출력
    //std::cout << "[" << inet_ntoa(clientAddress.sin_addr) << ":" << ntohs(clientAddress.sin_port) << "] 전송받는 파일 : " << name << " 전송받는 파일 크기 : " << fileSize << "Byte" << std::endl;

    //// 파일 받는 로직
    //FILE* fp = fopen(name, "wb+");
    //if (fp == NULL)
    //{
    //    std::cout << "파일 쓰기 오류. 해당 파일 생략" << std::endl;
    //    return;
    //}

    //memset(buf, 0, sizeof(buf));
    //int readSize = 0;
    //int totalSize = 0;
    //while ((readSize = recv(socketArray[index], buf, BUF_SIZE, 0)) != 0)
    //{
    //    if (GetLastError() == WSAEWOULDBLOCK)
    //    {
    //        Sleep(50); // 잠시 기다렸다가 재전송
    //        if (totalSize == fileSize)
    //        {
    //            std::cout << "파일 받기 완료" << std::endl;
    //            break;
    //        }
    //        continue;
    //    }
    //    totalSize += readSize;
    //    fwrite(buf, 1, readSize, fp);
    //}
    //fclose(fp);
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
