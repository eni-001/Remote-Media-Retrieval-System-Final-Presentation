#pragma once

#include <string>
#include <vector>
#include "FileRecord.h"

class ClientController
{
private:
    bool connected;
    bool authenticated;

public:
    ClientController();

    bool ConnectToServer(const std::string& ip, const std::string& port);
    bool Authenticate(const std::string& username, const std::string& password);
    std::vector<FileRecord> GetMockFileList() const;

    bool IsConnected() const;
    bool IsAuthenticated() const;
};