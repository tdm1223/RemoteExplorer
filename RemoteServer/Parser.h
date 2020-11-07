#pragma once
#include "packet.h"
#include<iostream>

#define BUF_SIZE 4096

class Parser
{
public:
    Packet Parsing(char recvBuffer[BUF_SIZE], int byteLen);

    // ���ۿ��� ����� �м��� ���� ������ �Ǻ��ϱ����� ����
    bool isHeaderPacket = false;

    // ��� ó�� ����
    char headerBuffer[BUF_SIZE];

    // �ý��ۿ� ��� ���� ����
    char waitBuffer[BUF_SIZE];

    // data ������ ������
    char* dataSortingIndex = 0;

    // ��� ���� ���ҿ� ����
    char tmpBuffer[BUF_SIZE];

    // ������ �ε��� ������
    char* recvBufferIndex = 0;

    // �޼��� ó�� ����
    char msgBuffer[BUF_SIZE];
};
