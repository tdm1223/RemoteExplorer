#include "NetworkContext.h"
#include <string>

NetworkContext::NetworkContext(OperationType operationType)
{
    ioData_ = nullptr;

    CleanContext();
    SetOperationType(operationType);
}

NetworkContext::~NetworkContext()
{
    if (ioData_ != nullptr)
    {
        delete ioData_;
    }
}

void NetworkContext::CleanContext()
{
    if (ioData_ != nullptr)
    {
        delete ioData_;
    }
    ioData_ = new IoData;
    SecureZeroMemory(&ioData_->overlapped, sizeof(OVERLAPPED));

    SecureZeroMemory(ioData_->buffer, NetworkContext::kMaxBufferSize);
    ioData_->wsaBuffer.buf = ioData_->buffer;
    ioData_->wsaBuffer.len = 0;
}

// Get, Set functions
OVERLAPPED* NetworkContext::GetOverlappedPointer()
{
    return reinterpret_cast<OVERLAPPED*>(ioData_);
}

void NetworkContext::SetOverlappedPointer(OVERLAPPED* overlapped)
{
    if (ioData_ != nullptr)
    {
        delete ioData_;
    }
    ioData_ = reinterpret_cast<IoData*>(overlapped);
}

WSABUF* NetworkContext::GetWsaBufferPointer()
{
    return &ioData_->wsaBuffer;
}

void NetworkContext::SetWsaBufferLength(int32_t length)
{
    ioData_->wsaBuffer.len = length;
}

int32_t NetworkContext::GetWsaBufferLength() const
{
    return ioData_->wsaBuffer.len;
}

char* NetworkContext::GetWsaBuffer() const
{
    return ioData_->wsaBuffer.buf;
}

void NetworkContext::SetBuffer(char* message, int32_t length)
{
    memcpy_s(ioData_->buffer, length, message, length);
    ioData_->wsaBuffer.len = length;
    ioData_->wsaBuffer.buf = ioData_->buffer;
}

void NetworkContext::SetOperationType(OperationType type)
{
    ioData_->operationType = type;

    // kLength 메세지의 경우 크기가 고정되어 있으므로 바꿔준다
    if (type == NetworkContext::OperationType::kLength)
    {
        SetWsaBufferLength(kLengthMessageSize);
    }
}

NetworkContext::OperationType NetworkContext::GetOperationType() const
{
    return ioData_->operationType;
}

// 직렬화 관련 함수
void NetworkContext::SerializeInt(const int32_t input, char* output)
{
    SecureZeroMemory(output, 4);
    int32_t* intPointer = reinterpret_cast<int32_t*>(output);
    *intPointer = input;
}

int32_t NetworkContext::DeserializeInt(const char* input)
{
    int32_t output = 0;

    char* charPointer = reinterpret_cast<char*>(&output);
    for (int32_t i = 0; i < 3; i++)
    {
        *charPointer = input[i];
        ++charPointer;
    }
    *charPointer = input[3];
    return output;
}
