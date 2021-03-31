#include"DownloadCommand.h"
#include"Util.h"

bool DownloadCommand::Execute(SOCKET sock, char* buf)
{
    std::cout << "Download" << std::endl;

    int cnt = 0;

    // RECV
    // 파일 전송
    int fileSize = 0;

    FILE* fp = fopen(buf, "rb");
    if (fp == NULL)
    {
        std::cout << "없는 파일" << std::endl;
    }
    else
    {
        std::cout << "있는 파일" << std::endl;
        fseek(fp, 0, SEEK_END);
        fileSize = ftell(fp);
    }

    fclose(fp);
    char buffer[Util::kLengthSize];
    SerializeInt(fileSize, buffer);
    send(sock, buffer, Util::kLengthSize, 0);

    // 파일 크기가 0보다 크다면 파일 전송

    // 0이면 파일이 없는것
    return true;
}