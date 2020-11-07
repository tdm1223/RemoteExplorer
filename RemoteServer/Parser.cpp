#include "parser.h"

/// <summary>
/// 받은 데이터를 파싱하는 함수
/// </summary>
/// <param name="recvBuffer">recv 함수를 통해 받은 버퍼</param>
/// <param name="byteLen">recv 함수를 통해 받은 버퍼의 크기</param>
/// <returns>파싱결과 패킷</returns>
Packet Parser::Parsing(char recvBuffer[BUF_SIZE], int byteLen)
{
    // 데이터를 처리할 수 있는 길이가 되었는지 확인용 변수
    // 현재 대기 버퍼안에 있는 데이터의 길이를 저장
    // 대기버퍼에 있는 데이터 길이 초기화
    int curLen = 0;

    // 메세지 헤더 구조체 포인터 선언
    Packet* recvHeaderPacket = new Packet();

    Packet resultPacket;

    // 대기 버퍼 초기화
    memset(waitBuffer, 0, BUF_SIZE);

    // 버퍼 포인터 초기화 대기 버퍼의 시작 주소를 가진다.
    recvBufferIndex = waitBuffer;

    // 길이만큼만 memcpy를 통해 저장
    memcpy(recvBufferIndex, recvBuffer, byteLen);

    curLen += byteLen;
    recvBufferIndex += byteLen;

    // 헤더 분석
    if (!isHeaderPacket)
    {
        // 헤더 길이 이상의 data가 있는지 확인
        if (curLen >= sizeof(recvHeaderPacket))
        {
            // 헤더 길이 이상 존재한다면 헤더 버퍼에 저장
            memset(headerBuffer, 0, sizeof(headerBuffer));
            memcpy(headerBuffer, waitBuffer, sizeof(recvHeaderPacket));
            headerBuffer[sizeof(Packet)] = '\0';
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
        }
    }

    std::cout << "recv message : " << msgBuffer << std::endl;
    resultPacket.command = recvHeaderPacket->command;
    resultPacket.size = recvHeaderPacket->size;
    return resultPacket;
}
