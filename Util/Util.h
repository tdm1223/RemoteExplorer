class Util
{
public:
    static const int kBufferSize = 4096;
    static const int kMaxFileNameLength = 520;
    static const int kLengthSize = 4;
    static const int kPort = 9000;
    static const int kPrefixSize = 8;
    static const int kCommandSize = 4;
    enum COMMAND
    {
        ECHO = 1,
        DOWNLOAD = 2,
        LIST = 3,
        END = 4
    };
};
