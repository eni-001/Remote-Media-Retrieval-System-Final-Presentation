#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

class TcpClient
{
private:
    SOCKET clientSocket;
    bool initialized;
    bool connected;
    std::string receiveBuffer;

public:
    TcpClient();
    ~TcpClient();

    bool Initialize();
    bool Connect(const std::string& ipAddress, int port);
    bool SendMessage(const std::string& message);
    std::string ReceiveMessage();
    void Disconnect();

    bool IsConnected() const;
};