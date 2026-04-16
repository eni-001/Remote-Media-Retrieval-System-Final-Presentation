#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

class TcpServer
{
private:
    SOCKET listenSocket;
    SOCKET clientSocket;
    bool initialized;
    bool running;
    std::string receiveBuffer;

public:
    TcpServer();
    ~TcpServer();

    bool Initialize();
    bool Start(int port);
    bool AcceptClient();
    std::string ReceiveMessage();
    bool SendMessage(const std::string& message);
    void Stop();

    bool IsRunning() const;
    bool HasClient() const;
};