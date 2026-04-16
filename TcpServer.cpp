#include "TcpServer.h"

#pragma comment(lib, "Ws2_32.lib")

TcpServer::TcpServer()
    : listenSocket(INVALID_SOCKET),
    clientSocket(INVALID_SOCKET),
    initialized(false),
    running(false),
    receiveBuffer("")
{
}

TcpServer::~TcpServer()
{
    Stop();
}

bool TcpServer::Initialize()
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

bool TcpServer::Start(int port)
{
    if (!initialized && !Initialize())
    {
        return false;
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET)
    {
        return false;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(static_cast<u_short>(port));

    int result = bind(listenSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
    if (result == SOCKET_ERROR)
    {
        closesocket(listenSocket);
        listenSocket = INVALID_SOCKET;
        return false;
    }

    result = listen(listenSocket, 1);
    if (result == SOCKET_ERROR)
    {
        closesocket(listenSocket);
        listenSocket = INVALID_SOCKET;
        return false;
    }

    running = true;
    return true;
}

bool TcpServer::AcceptClient()
{
    if (!running)
    {
        return false;
    }

    clientSocket = accept(listenSocket, nullptr, nullptr);
    return clientSocket != INVALID_SOCKET;
}

std::string TcpServer::ReceiveMessage()
{
    if (clientSocket == INVALID_SOCKET)
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

bool TcpServer::SendMessage(const std::string& message)
{
    if (clientSocket == INVALID_SOCKET)
    {
        return false;
    }

    std::string framed = message + "\n";
    int result = send(clientSocket, framed.c_str(), static_cast<int>(framed.size()), 0);
    return result != SOCKET_ERROR;
}

void TcpServer::Stop()
{
    if (clientSocket != INVALID_SOCKET)
    {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
    }

    if (listenSocket != INVALID_SOCKET)
    {
        closesocket(listenSocket);
        listenSocket = INVALID_SOCKET;
    }

    running = false;

    if (initialized)
    {
        WSACleanup();
        initialized = false;
    }

    receiveBuffer.clear();
}

bool TcpServer::IsRunning() const
{
    return running;
}

bool TcpServer::HasClient() const
{
    return clientSocket != INVALID_SOCKET;
}