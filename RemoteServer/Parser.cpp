#include "parser.h"

/// <summary>
/// ���� �����͸� �Ľ��ϴ� �Լ�
/// </summary>
/// <param name="recvBuffer">recv �Լ��� ���� ���� ����</param>
/// <param name="byteLen">recv �Լ��� ���� ���� ������ ũ��</param>
/// <returns>�Ľ̰�� ��Ŷ</returns>
Packet Parser::Parsing(char recvBuffer[BUF_SIZE], int byteLen)
{
    // �����͸� ó���� �� �ִ� ���̰� �Ǿ����� Ȯ�ο� ����
    // ���� ��� ���۾ȿ� �ִ� �������� ���̸� ����
    // �����ۿ� �ִ� ������ ���� �ʱ�ȭ
    int curLen = 0;

    // �޼��� ��� ����ü ������ ����
    Packet* recvHeaderPacket = new Packet();

    Packet resultPacket;

    // ��� ���� �ʱ�ȭ
    memset(waitBuffer, 0, BUF_SIZE);

    // ���� ������ �ʱ�ȭ ��� ������ ���� �ּҸ� ������.
    recvBufferIndex = waitBuffer;

    // ���̸�ŭ�� memcpy�� ���� ����
    memcpy(recvBufferIndex, recvBuffer, byteLen);

    curLen += byteLen;
    recvBufferIndex += byteLen;

    // ��� �м�
    if (!isHeaderPacket)
    {
        // ��� ���� �̻��� data�� �ִ��� Ȯ��
        if (curLen >= sizeof(recvHeaderPacket))
        {
            // ��� ���� �̻� �����Ѵٸ� ��� ���ۿ� ����
            memset(headerBuffer, 0, sizeof(headerBuffer));
            memcpy(headerBuffer, waitBuffer, sizeof(recvHeaderPacket));
            headerBuffer[sizeof(Packet)] = '\0';
            recvHeaderPacket = (Packet*)headerBuffer;

            // ������ data�� ����
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
        // �м��� ����� �����ߴ� �޽��� ���̸�ŭ�� �����Ͱ� �����ϴ��� Ȯ��
        if (curLen >= recvHeaderPacket->size)
        {
            // �����Ͱ� �����Ѵٸ� msgBuffer�� ����
            memset(msgBuffer, 0, sizeof(msgBuffer));
            memcpy(msgBuffer, waitBuffer, recvHeaderPacket->size);
            msgBuffer[recvHeaderPacket->size] = '\0';

            // ������ data�� ����
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
