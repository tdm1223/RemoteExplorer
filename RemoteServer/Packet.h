#pragma once
#include<vector>
#include<winsock.h>
#include<iostream>

class Uncopyable 
{
protected:
    // packet 클래스의 생성과 소멸은 허용
    Uncopyable() {};
    ~Uncopyable() {};
private:
    // packet 클래스의 객체 복사 방지
    Uncopyable(const Uncopyable&);
    Uncopyable& operator=(const Uncopyable&) {};
};

class Packet : private Uncopyable
{
public:
    virtual const bool Parsing(const char* buffer, const unsigned int bufferSize) = 0;
    virtual const bool Building(char* buffer, unsigned int& buildBufferSize) = 0;
    virtual const void Clear() = 0;
    virtual const int GetHeaderSize() = 0;

    const std::string kPrefix = "ESTSOFT";
};

class CustomPacket : public Packet
{
public:
    enum { UPLOAD = 1, DOWNLOAD = 2, END = 3 };
    void SetCommand(int command);
    void SetSize(int size);
    void SetPrefix(const char* prefix);
    void SetData(std::vector<char> data);
    const int GetCommand() const;
    const int GetSize() const;
    const char* GetPrefix() const;
    const std::vector<char> GetData() const;
    virtual const bool Parsing(const char* buffer, const unsigned int bufferSize) override;
    virtual const bool Building(char* buffer, unsigned int& buildBufferSize) override;
    virtual const void Clear() override;
    virtual const int GetHeaderSize() override;

private:
    char prefix[8];
    int command;
    int size;
    std::vector<char> data;
};