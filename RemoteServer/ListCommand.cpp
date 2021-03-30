#include"ListCommand.h"
#include"Util.h"

bool ListCommand::Execute(SOCKET sock, char* buf)
{
    std::cout << "LIST" << std::endl;
    int cnt = 0;
    std::vector<std::string> fileVector;
    std::string files;
    for (const fs::directory_entry& entry : fs::directory_iterator(fs::current_path()))
    {
        files = entry.path().string();
        size_t pos = files.rfind("\\");
        files = files.substr(pos + 1);
        fileVector.push_back(files);
        cnt++;
    }

    // 파일 개수를 보냄
    if (!SendLength(sock, cnt))
    {
        return false;
    }

    // 개수만큼 파일명을 보냄
    char sendBuf[Util::kMaxFileNameLength];
    for (auto& file : fileVector)
    {
        // 파일명 전송
        if (!Send(sock, file.c_str()))
        {
            return false;
        }
    }
    return true;
}