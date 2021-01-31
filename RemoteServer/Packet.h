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
    virtual const bool OnParse(const char* buffer, const unsigned int bufferSize) = 0;
    virtual const bool OnBuild(char* buffer, unsigned int& buildBufferSize) = 0;
    virtual const void Clear() = 0;
    virtual const int GetHeaderSize() = 0;

    const std::string kPrefix = "ESTSOFT";
};