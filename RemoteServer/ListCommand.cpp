#include"ListCommand.h"
#include"Util.h"

bool ListCommand::Execute(SOCKET& sock)
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

    // ���� ������ ����
    if (!SendLength(sock, cnt))
    {
        return false;
    }

    // ������ŭ ���ϸ��� ����
    char sendBuf[Util::kMaxFileNameLength];
    for (auto& file : fileVector)
    {
        // ���ϸ� ����
        strcpy(sendBuf, file.c_str());
        if (!Send(sock, sendBuf))
        {
            return false;
        }
    }
    return true;
}