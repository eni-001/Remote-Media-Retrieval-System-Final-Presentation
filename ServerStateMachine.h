#pragma once

#include <string>

enum class ServerState
{
    Startup,
    WaitingForAuth,
    AuthenticatedReady,
    Transferring,
    Error
};

class ServerStateMachine
{
private:
    ServerState currentState;

public:
    ServerStateMachine();

    ServerState GetCurrentState() const;
    std::string GetStateAsString() const;

    void SetState(ServerState newState);
};