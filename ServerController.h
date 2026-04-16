#pragma once

#include <string>
#include "ServerStateMachine.h"

class ServerController
{
private:
    bool running;
    ServerStateMachine stateMachine;

public:
    ServerController();

    bool StartServer(const std::string& port);
    void StopServer();

    bool AuthenticateClient(const std::string& username, const std::string& password);
    bool BeginTransfer();

    bool IsRunning() const;
    std::string GetCurrentState() const;
};