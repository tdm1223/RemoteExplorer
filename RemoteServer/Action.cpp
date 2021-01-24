#include "Action.h"
#include "Upload.h"
#include "Download.h"
#include "Test.h"

Action* GetActionPtr(const std::string& action_str)
{
    if (action_str == "UPLOAD")
    {
        std::cout << "UPLOAD 생성 완료" << std::endl;
        return (Action*)new Upload;
    }
    else if (action_str == "DOWNLOAD")
    {
        std::cout << "DOWNLOAD 생성 완료" << std::endl;
        return (Action*) new Download;
    }
    else if (action_str == "TEST")
    {
        std::cout << "TEST 생성 완료" << std::endl;
        return (Action*) new Test;
    }
}