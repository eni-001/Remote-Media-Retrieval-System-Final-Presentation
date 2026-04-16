#include "ServerController.h"

#include <cstdlib>

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

    int portNumber = std::atoi(port.c_str());
    if (portNumber <= 0)
    {
        stateMachine.SetState(ServerState::Error);
        return false;
    }

    if (!tcpServer.Start(portNumber))
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
    tcpServer.Stop();
    running = false;
    stateMachine.SetState(ServerState::Startup);
}

bool ServerController::AcceptClient()
{
    if (!running)
    {
        return false;
    }

    return tcpServer.AcceptClient();
}

std::string ServerController::ReceiveClientMessage()
{
    return tcpServer.ReceiveMessage();
}

bool ServerController::SendClientMessage(const std::string& message)
{
    return tcpServer.SendMessage(message);
}

std::string ServerController::ProcessClientMessage(const std::string& message)
{
    if (message.rfind("AUTH|", 0) == 0)
    {
        stateMachine.SetState(ServerState::AuthenticatedReady);
        return "AUTH_OK";
    }

    if (message == "LIST_FILES")
    {
        if (stateMachine.GetCurrentState() != ServerState::AuthenticatedReady)
        {
            stateMachine.SetState(ServerState::Error);
            return "ERROR|NOT_AUTHENTICATED";
        }

        return "image1.jpg|JPEG|245760\nimage2.jpg|JPEG|532480\nsample_large_image.jpg|JPEG|1572864";
    }

    if (message.rfind("GET_IMAGE|", 0) == 0)
    {
        if (stateMachine.GetCurrentState() != ServerState::AuthenticatedReady)
        {
            stateMachine.SetState(ServerState::Error);
            return "ERROR|NOT_AUTHENTICATED";
        }

        stateMachine.SetState(ServerState::Transferring);
        std::string response = "IMAGE_REQUEST_ACCEPTED";
        stateMachine.SetState(ServerState::AuthenticatedReady);
        return response;
    }

    stateMachine.SetState(ServerState::Error);
    return "ERROR|INVALID_COMMAND";
}

bool ServerController::IsRunning() const
{
    return running;
}

std::string ServerController::GetCurrentState() const
{
    return stateMachine.GetStateAsString();
}