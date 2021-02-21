#ifndef RECEIVER_H
#define RECEIVER_H

#include "PacketCommand.h"
#include "UploadCommand.h"
#include "DownloadCommand.h"
#include<vector>

class Server;

class Receiver {
public:
    Receiver();
    void Start();
    void operator()(std::vector<std::unique_ptr<PacketCommand>>* commands);
};

#endif