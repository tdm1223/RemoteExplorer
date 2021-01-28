#include "Action.h"
#include "Upload.h"
#include "Download.h"
#include "Test.h"

Action* GetAction(const std::string& action)
{
    if (action == "UPLOAD")
    {
        std::cout << "UPLOAD 생성 완료" << std::endl;
        return (Action*)new Upload;
    }
    else if (action == "DOWNLOAD")
    {
        std::cout << "DOWNLOAD 생성 완료" << std::endl;
        return (Action*) new Download;
    }
    else if (action == "TEST")
    {
        std::cout << "TEST 생성 완료" << std::endl;
        return (Action*) new Test;
    }
}