#include "TcpClient.h"

#pragma comment(lib, "Ws2_32.lib")

TcpClient::TcpClient()
    : clientSocket(INVALID_SOCKET), initialized(false), connected(false), receiveBuffer("")
{
}

TcpClient::~TcpClient()
{
    Disconnect();
}

bool TcpClient::Initialize()
{
    if (initialized)
    {
        return true;
    }

    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        return false;
    }

    initialized = true;
    return true;
}

bool TcpClient::Connect(const std::string& ipAddress, int port)
{
    if (!initialized && !Initialize())
    {
        return false;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(static_cast<u_short>(port));

    int result = inet_pton(AF_INET, ipAddress.c_str(), &serverAddr.sin_addr);
    if (result <= 0)
    {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        return false;
    }

    result = connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
    if (result == SOCKET_ERROR)
    {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        return false;
    }

    connected = true;
    return true;
}

bool TcpClient::SendMessage(const std::string& message)
{
    if (!connected)
    {
        return false;
    }

    std::string framed = message + "\n";
    int result = send(clientSocket, framed.c_str(), static_cast<int>(framed.size()), 0);
    return result != SOCKET_ERROR;
}

std::string TcpClient::ReceiveMessage()
{
    if (!connected)
    {
        return "";
    }

    while (true)
    {
        std::size_t newlinePos = receiveBuffer.find('\n');
        if (newlinePos != std::string::npos)
        {
            std::string message = receiveBuffer.substr(0, newlinePos);
            receiveBuffer.erase(0, newlinePos + 1);
            return message;
        }

        char buffer[4096] = {};
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0)
        {
            return "";
        }

        receiveBuffer.append(buffer, bytesReceived);
    }
}

void TcpClient::Disconnect()
{
    if (clientSocket != INVALID_SOCKET)
    {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
    }

    connected = false;

    if (initialized)
    {
        WSACleanup();
        initialized = false;
    }

    receiveBuffer.clear();
}

bool TcpClient::IsConnected() const
{
    return connected;
}