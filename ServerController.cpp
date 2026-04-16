#include "ServerController.h"

#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <filesystem>

#include "PacketFactory.h"
#include "CommandType.h"
#include "FileChunkHelper.h"

namespace fs = std::filesystem;

ServerController::ServerController()
    : running(false)
{
}

// Convert binary chunk to hex text so it is safe to send inside packets
std::string ToHex(const std::string& input)
{
    std::ostringstream oss;
    for (unsigned char c : input)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    return oss.str();
}

bool ServerController::StartServer(const std::string& port)
{
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
    return tcpServer.AcceptClient();
}

Packet ServerController::ReceivePacket()
{
    std::string receivedData = tcpServer.ReceiveMessage();

    if (receivedData.empty())
    {
        return Packet();
    }

    return Packet::Deserialize(receivedData);
}

bool ServerController::SendPacket(const Packet& packet)
{
    return tcpServer.SendMessage(packet.Serialize());
}

Packet ServerController::ProcessPacket(const Packet& packet)
{
    if (packet.command == CommandType::Authenticate)
    {
        stateMachine.SetState(ServerState::AuthenticatedReady);
        return PacketFactory::CreateResponsePacket(CommandType::Response, "AUTH_OK", 1);
    }

    if (packet.command == CommandType::RequestFileList)
    {
        std::string payload =
            "image1.jpg|JPEG|245760##"
            "image2.jpg|JPEG|532480##"
            "sample_large_image.jpg|JPEG|1572864";

        return PacketFactory::CreateResponsePacket(CommandType::Response, payload, 1);
    }

    if (packet.command == CommandType::GetImage)
    {
        stateMachine.SetState(ServerState::Transferring);

        if (SendFileChunks(packet.payload))
        {
            stateMachine.SetState(ServerState::AuthenticatedReady);
            return PacketFactory::CreateTransferCompletePacket(1);
        }

        stateMachine.SetState(ServerState::Error);
        return PacketFactory::CreateErrorPacket("ERROR|FILE_TRANSFER_FAILED", 1);
    }

    stateMachine.SetState(ServerState::Error);
    return PacketFactory::CreateErrorPacket("ERROR|INVALID_COMMAND", 1);
}

bool ServerController::SendFileChunks(const std::string& fileName)
{
    fs::path currentDir = fs::current_path();

    std::vector<fs::path> candidates =
    {
        currentDir / "ServerFiles" / fileName,
        currentDir / "RemoteMediaRetrievalSystem" / "ServerFiles" / fileName,
        currentDir.parent_path() / "ServerFiles" / fileName,
        currentDir.parent_path().parent_path() / "ServerFiles" / fileName,
        fs::path("C:\\Users\\eenio\\OneDrive\\Desktop\\ServerFiles") / fileName,
        fs::path("C:\\Users\\eenio\\OneDrive\\Desktop\\RemoteMediaRetrievalSystem\\ServerFiles") / fileName
    };

    std::cout << "Current directory: " << currentDir.string() << "\n";

    fs::path foundPath;
    bool found = false;

    for (const auto& p : candidates)
    {
        std::cout << "Trying file: " << p.string() << "\n";
        if (fs::exists(p) && fs::is_regular_file(p))
        {
            foundPath = p;
            found = true;
            break;
        }
    }

    if (!found)
    {
        std::cout << "File not found in any expected location.\n";
        return false;
    }

    std::cout << "Using file: " << foundPath.string() << "\n";

    std::vector<std::string> chunks =
        FileChunkHelper::ReadFileInChunks(foundPath.string(), 512);

    std::cout << "Chunks read: " << chunks.size() << "\n";

    if (chunks.empty())
    {
        std::cout << "File read failed or file is empty.\n";
        return false;
    }

    for (int i = 0; i < static_cast<int>(chunks.size()); i++)
    {
        std::string safeChunk = ToHex(chunks[i]);

        Packet chunkPacket =
            PacketFactory::CreateTransferChunkPacket(safeChunk, i, 1);

        if (!SendPacket(chunkPacket))
        {
            std::cout << "Failed sending chunk " << i << "\n";
            return false;
        }

        std::cout << "Sent chunk " << i
            << " | Raw size: " << chunks[i].size()
            << " | Hex size: " << safeChunk.size() << "\n";
    }

    std::cout << "All chunks sent successfully.\n";
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