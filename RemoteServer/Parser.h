#pragma once
#include "packet.h"
#include<iostream>

#define BUF_SIZE 4096

class Parser
{
public:
    Packet Parsing(char recvBuffer[BUF_SIZE], int byteLen);

    // 버퍼에서 헤더를 분석할 순서 인지를 판별하기위한 변수
    bool isHeaderPacket = false;

    // 헤더 처리 버퍼
    char headerBuffer[BUF_SIZE];

    // 시스템용 대기 버퍼 선언
    char waitBuffer[BUF_SIZE];

    // data 정리용 포인터
    char* dataSortingIndex = 0;

    // 대기 버퍼 스왑용 버퍼
    char tmpBuffer[BUF_SIZE];

    // 버퍼의 인덱스 포인터
    char* recvBufferIndex = 0;

    // 메세지 처리 버퍼
    char msgBuffer[BUF_SIZE];
};
