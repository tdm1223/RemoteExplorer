#ifndef NETWORK_NETWORKCONTEXT_H
#define NETWORK_NETWORKCONTEXT_H

#include <WinSock2.h>
#include <cinttypes>

class NetworkContext
{
public:
    /// �޼��� ����
    enum class OperationType {
        kSend = 0,  ///< ��Ŷ ����
        kRecv,      ///< ���� ��� ����
        kLength    ///< �޼��� ���� ����
    };

    static const int32_t kMaxBufferSize = 4096;             // ���� ���� �ִ� ������
    static const int32_t kLengthMessageSize = 4;            // �޼��� ���� ��Ŷ ũ��
    static const int32_t kStopSignalCode = -171171171;      // �۽� ������ �˸� �ڵ�

    NetworkContext(OperationType operationType);
    ~NetworkContext();

    NetworkContext(const NetworkContext&) = delete;
    NetworkContext& operator=(const NetworkContext&) = delete;

    void CleanContext();  ///< ioData ��� �����͸� ���Ҵ��ϰ� �ʱ�ȭ�Ѵ�.

    // Get, Set functions
    OVERLAPPED* GetOverlappedPointer();
    void SetOverlappedPointer(OVERLAPPED* overlapped);

    WSABUF* GetWsaBufferPointer();
    void SetWsaBufferLength(int32_t length);
    int32_t GetWsaBufferLength() const;
    char* GetWsaBuffer() const;

    void SetBuffer(char* message, int32_t length);

    void SetOperationType(OperationType type);
    OperationType GetOperationType() const;

    // ����ȭ ���� �Լ�
    static void SerializeInt(const int32_t input, char* output);
    static int32_t DeserializeInt(const char* input);

private:
    // WSASend, WSARecv ��� ���޵� ���� overlapped ����ü
    struct IoData
    {
        OVERLAPPED overlapped;
        OperationType operationType;
        WSABUF wsaBuffer;
        char buffer[NetworkContext::kMaxBufferSize];
    };

    IoData* ioData_;
};

#endif