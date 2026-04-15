#include "ClientController.h"

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

    authenticated = true;
    return true;
}

std::vector<FileRecord> ClientController::GetMockFileList() const
{
    if (!authenticated)
    {
        return {};
    }

    return {
        FileRecord("image1.jpg", "JPEG", 245760),
        FileRecord("image2.jpg", "JPEG", 532480),
        FileRecord("sample_large_image.jpg", "JPEG", 1572864)
    };
}

bool ClientController::IsConnected() const
{
    return connected;
}

bool ClientController::IsAuthenticated() const
{
    return authenticated;
}