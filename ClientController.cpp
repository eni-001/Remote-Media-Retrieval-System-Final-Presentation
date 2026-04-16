#include "ClientController.h"

#include <cstdlib>

ClientController::ClientController()
    : connected(false), authenticated(false)
{
}

bool ClientController::ConnectToServer(const std::string& ip, const std::string& port)
{
    if (ip.empty() || port.empty())
    {
        return false;
    }

    int portNumber = std::atoi(port.c_str());
    if (portNumber <= 0)
    {
        return false;
    }

    if (!tcpClient.Connect(ip, portNumber))
    {
        connected = false;
        return false;
    }

    connected = true;
    return true;
}

bool ClientController::Authenticate(const std::string& username, const std::string& password)
{
    if (!connected)
    {
        return false;
    }

    if (username.empty() || password.empty())
    {
        return false;
    }

    return true;
}

bool ClientController::SendPacket(const Packet& packet)
{
    if (!connected)
    {
        return false;
    }

    return tcpClient.SendMessage(packet.Serialize());
}

Packet ClientController::ReceivePacket()
{
    if (!connected)
    {
        return Packet();
    }

    std::string response = tcpClient.ReceiveMessage();
    if (response.empty())
    {
        return Packet();
    }

    return Packet::Deserialize(response);
}

void ClientController::Disconnect()
{
    tcpClient.Disconnect();
    connected = false;
    authenticated = false;
}

bool ClientController::IsConnected() const
{
    return connected;
}

bool ClientController::IsAuthenticated() const
{
    return authenticated;
}

void ClientController::SetAuthenticated(bool value)
{
    authenticated = value;
}