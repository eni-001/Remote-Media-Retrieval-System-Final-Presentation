#pragma once

#include <string>

#include "TcpClient.h"
#include "Packet.h"

class ClientController
{
private:
    bool connected;
    bool authenticated;
    TcpClient tcpClient;

public:
    ClientController();

    bool ConnectToServer(const std::string& ip, const std::string& port);
    bool Authenticate(const std::string& username, const std::string& password);

    bool SendPacket(const Packet& packet);
    Packet ReceivePacket();

    void Disconnect();

    bool IsConnected() const;
    bool IsAuthenticated() const;
    void SetAuthenticated(bool value);
};