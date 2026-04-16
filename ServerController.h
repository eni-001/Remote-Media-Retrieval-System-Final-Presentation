#pragma once

#include <string>
#include <vector>

#include "ServerStateMachine.h"
#include "TcpServer.h"
#include "Packet.h"

class ServerController
{
private:
    bool running;
    TcpServer tcpServer;
    ServerStateMachine stateMachine;

public:
    ServerController();

    bool StartServer(const std::string& port);
    void StopServer();

    bool AcceptClient();
    Packet ReceivePacket();
    bool SendPacket(const Packet& packet);

    Packet ProcessPacket(const Packet& packet);
    bool SendFileChunks(const std::string& fileName);

    bool IsRunning() const;
    std::string GetCurrentState() const;
};