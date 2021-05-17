#ifndef NETWORK_NETWORKCONTEXT_H
#define NETWORK_NETWORKCONTEXT_H

#include <WinSock2.h>
#include <cinttypes>

class NetworkContext
{
public:
    /// 메세지 유형
    enum class OperationType {
        kSend = 0,  ///< 패킷 전송
        kRecv,      ///< 파일 경로 수신
        kLength    ///< 메세지 길이 수신
    };

    static const int32_t kMaxBufferSize = 4096;             // 소켓 버퍼 최대 사이즈
    static const int32_t kLengthMessageSize = 4;            // 메세지 길이 패킷 크기
    static const int32_t kStopSignalCode = -171171171;      // 송신 끝남을 알릴 코드

    NetworkContext(OperationType operationType);
    ~NetworkContext();

    NetworkContext(const NetworkContext&) = delete;
    NetworkContext& operator=(const NetworkContext&) = delete;

    void CleanContext();  ///< ioData 멤버 포인터를 재할당하고 초기화한다.

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

    // 직렬화 관련 함수
    static void SerializeInt(const int32_t input, char* output);
    static int32_t DeserializeInt(const char* input);

private:
    // WSASend, WSARecv 등에서 전달될 실제 overlapped 구조체
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