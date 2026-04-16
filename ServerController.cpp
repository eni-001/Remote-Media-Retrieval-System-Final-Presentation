#include "ServerController.h"

ServerController::ServerController()
    : running(false)
{
}

bool ServerController::StartServer(const std::string& port)
{
    if (port.empty())
    {
        stateMachine.SetState(ServerState::Error);
        return false;
    }

    running = true;
    stateMachine.SetState(ServerState::WaitingForAuth);
    return true;
}

void ServerController::StopServer()
{
    running = false;
    stateMachine.SetState(ServerState::Startup);
}

bool ServerController::AuthenticateClient(const std::string& username, const std::string& password)
{
    if (!running || username.empty() || password.empty())
    {
        stateMachine.SetState(ServerState::Error);
        return false;
    }

    stateMachine.SetState(ServerState::AuthenticatedReady);
    return true;
}

bool ServerController::BeginTransfer()
{
    if (!running || stateMachine.GetCurrentState() != ServerState::AuthenticatedReady)
    {
        stateMachine.SetState(ServerState::Error);
        return false;
    }

    stateMachine.SetState(ServerState::Transferring);
    return true;
}

bool ServerController::IsRunning() const
{
    return running;
}

std::string ServerController::GetCurrentState() const
{
    return stateMachine.GetStateAsString();
}