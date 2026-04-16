#include "ServerStateMachine.h"

ServerStateMachine::ServerStateMachine()
    : currentState(ServerState::Startup)
{
}

ServerState ServerStateMachine::GetCurrentState() const
{
    return currentState;
}

std::string ServerStateMachine::GetStateAsString() const
{
    switch (currentState)
    {
    case ServerState::Startup:
        return "STARTUP";
    case ServerState::WaitingForAuth:
        return "WAITING_FOR_AUTH";
    case ServerState::AuthenticatedReady:
        return "AUTHENTICATED_READY";
    case ServerState::Transferring:
        return "TRANSFERRING";
    case ServerState::Error:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}

void ServerStateMachine::SetState(ServerState newState)
{
    currentState = newState;
}