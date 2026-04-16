#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <cstdlib>

#include "ClientController.h"
#include "Logger.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "CommandType.h"
#include "FileChunkHelper.h"

void DisplayMenu()
{
    std::cout << "\n===== Client Application Menu =====\n";
    std::cout << "1. Connect to Server\n";
    std::cout << "2. Authenticate\n";
    std::cout << "3. Request File List\n";
    std::cout << "4. Get Image\n";
    std::cout << "5. Exit\n";
    std::cout << "Select an option: ";
}

std::string HexToBinary(const std::string& hex)
{
    std::string output;

    for (size_t i = 0; i + 1 < hex.length(); i += 2)
    {
        std::string byteString = hex.substr(i, 2);
        char byte = static_cast<char>(std::strtol(byteString.c_str(), nullptr, 16));
        output.push_back(byte);
    }

    return output;

#include "ServerController.h"
#include "Logger.h"
#include "Packet.h"
#include "CommandType.h"

void DisplayServerMenu()
{
    std::cout << "\n===== Server Application Menu =====\n";
    std::cout << "1. Start Server\n";
    std::cout << "2. Wait for Client Connection\n";
    std::cout << "3. Process One Client Packet\n";
    std::cout << "4. Show Current State\n";
    std::cout << "5. Stop Server\n";
    std::cout << "6. Exit\n";
    std::cout << "Select an option: ";
}

std::string CommandToString(CommandType command)
{
    switch (command)
    {
    case CommandType::Authenticate:
        return "Authenticate";
    case CommandType::RequestFileList:
        return "RequestFileList";
    case CommandType::GetImage:
        return "GetImage";
    case CommandType::Response:
        return "Response";
    case CommandType::Error:
        return "Error";
    default:
        return "Unknown";
    }
}

int main()
{
    ClientController controller;
    const std::string logFile = "client_log.txt";

    int choice = 0;

    std::cout << "Remote Media Retrieval System - Client Application\n";

    while (true)
    {
        DisplayMenu();
    ServerController controller;
    const std::string logFile = "server_log.txt";

    int choice = 0;

    std::cout << "Remote Media Retrieval System - Server Application\n";

    while (true)
    {
        DisplayServerMenu();

        if (!(std::cin >> choice))
        {
            std::cin.clear();
            std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

        if (choice == 1)
        {
            std::string ip;
            std::string port;

            std::cout << "Enter Server IP: ";
            std::getline(std::cin, ip);

            std::cout << "Enter Server Port: ";
            std::getline(std::cin, port);

            if (controller.ConnectToServer(ip, port))
            {
                std::cout << "Connected to server successfully.\n";
                Logger::WriteTxLog(logFile, "Client connected to server.");
            }
            else
            {
                std::cout << "Connection failed.\n";
                Logger::WriteLog(logFile, "Client connection failed.");
            std::string port;
            std::cout << "Enter Server Port: ";
            std::getline(std::cin, port);

            if (controller.StartServer(port))
            {
                std::cout << "Server started successfully.\n";
                std::cout << "Current State: " << controller.GetCurrentState() << "\n";
                Logger::WriteLog(logFile, "Server started on port " + port);
            }
            else
            {
                std::cout << "Server failed to start.\n";
                Logger::WriteLog(logFile, "Server failed to start.");
            }
        }
        else if (choice == 2)
        {
            std::string username;
            std::string password;

            std::cout << "Enter Username: ";
            std::getline(std::cin, username);

            std::cout << "Enter Password: ";
            std::getline(std::cin, password);

            if (!controller.Authenticate(username, password))
            {
                std::cout << "Authentication failed before packet send.\n";
                Logger::WriteLog(logFile, "Client authentication validation failed.");
                continue;
            }

            Packet authPacket = PacketFactory::CreateAuthPacket(username, password);

            if (!controller.SendPacket(authPacket))
            {
                std::cout << "Failed to send authentication packet.\n";
                Logger::WriteLog(logFile, "Failed to send authentication packet.");
                continue;
            }

            Logger::WriteTxLog(logFile, "Sent AUTH packet.");

            Packet responsePacket = controller.ReceivePacket();

            if (responsePacket.command == CommandType::Response && responsePacket.payload == "AUTH_OK")
            {
                controller.SetAuthenticated(true);
                std::cout << "Authentication successful.\n";
                Logger::WriteRxLog(logFile, "Received AUTH_OK packet.");
            }
            else
            {
                controller.SetAuthenticated(false);
                std::cout << "Authentication failed.\n";
                Logger::WriteLog(logFile, "Authentication response packet invalid.");
            std::cout << "Waiting for client connection...\n";

            if (controller.AcceptClient())
            {
                std::cout << "Client connected successfully.\n";
                Logger::WriteRxLog(logFile, "Client connected to server.");
            }
            else
            {
                std::cout << "Failed to accept client connection.\n";
                Logger::WriteLog(logFile, "Client connection accept failed.");
            }
        }
        else if (choice == 3)
        {
            if (!controller.IsAuthenticated())
            {
                std::cout << "Client is not authenticated.\n";
                Logger::WriteLog(logFile, "File list request denied: client not authenticated.");
                continue;
            }

            Packet listPacket = PacketFactory::CreateListFilesPacket();

            if (!controller.SendPacket(listPacket))
            {
                std::cout << "Failed to send file list packet.\n";
                Logger::WriteLog(logFile, "Failed to send LIST_FILES packet.");
                continue;
            }

            Logger::WriteTxLog(logFile, "Sent LIST_FILES packet.");

            Packet responsePacket = controller.ReceivePacket();
            if (responsePacket.payload.empty())
            {
                std::cout << "No response from server.\n";
                Logger::WriteLog(logFile, "No response packet received for LIST_FILES.");
                continue;
            }

            std::cout << "\nServer Response:\n";

            std::string payload = responsePacket.payload;
            size_t start = 0;
            while (true)
            {
                size_t pos = payload.find("##", start);
                if (pos == std::string::npos)
                {
                    std::cout << payload.substr(start) << "\n";
                    break;
                }

                std::cout << payload.substr(start, pos - start) << "\n";
                start = pos + 2;
            }

            Logger::WriteRxLog(logFile, "Received file list response packet.");
        }
        else if (choice == 4)
        {
            if (!controller.IsAuthenticated())
            {
                std::cout << "Client is not authenticated.\n";
                Logger::WriteLog(logFile, "Get image denied: client not authenticated.");
                continue;
            }

            std::string requestedFile;
            std::cout << "Enter exact file name to request: ";
            std::getline(std::cin, requestedFile);

            if (requestedFile.empty())
            {
                std::cout << "Invalid file name.\n";
                Logger::WriteLog(logFile, "Get image failed: empty file name.");
                continue;
            }

            Packet imagePacket = PacketFactory::CreateGetImagePacket(requestedFile);

            if (!controller.SendPacket(imagePacket))
            {
                std::cout << "Failed to send GET_IMAGE packet.\n";
                Logger::WriteLog(logFile, "Failed to send GET_IMAGE packet.");
                continue;
            }

            Logger::WriteTxLog(logFile, "Sent GET_IMAGE packet for " + requestedFile);

            std::vector<std::string> receivedChunks;
            bool transferFinished = false;
            bool transferFailed = false;

            while (!transferFinished)
            {
                Packet incomingPacket = controller.ReceivePacket();

                if (incomingPacket.command == CommandType::None && incomingPacket.payload.empty())
                {
                    std::cout << "No packet received from server.\n";
                    Logger::WriteLog(logFile, "No packet received during image transfer.");
                    transferFailed = true;
                    break;
                }

                if (incomingPacket.command == CommandType::TransferChunk)
                {
                    std::string binaryChunk = HexToBinary(incomingPacket.payload);
                    receivedChunks.push_back(binaryChunk);

                    std::cout << "Received chunk #" << incomingPacket.sequenceNumber
                        << " | Size: " << binaryChunk.size() << " bytes\n";

                    Logger::WriteRxLog(
                        logFile,
                        "Received chunk #" + std::to_string(incomingPacket.sequenceNumber)
                    );
                }
                else if (incomingPacket.command == CommandType::TransferComplete)
                {
                    transferFinished = true;
                    Logger::WriteRxLog(logFile, "Received TRANSFER_COMPLETE packet.");
                }
                else if (incomingPacket.command == CommandType::Error)
                {
                    std::cout << "Server Response: " << incomingPacket.payload << "\n";
                    Logger::WriteLog(logFile, "Server returned error during image transfer: " + incomingPacket.payload);
                    transferFailed = true;
                    break;
                }
                else
                {
                    std::cout << "Unexpected packet received.\n";
                    Logger::WriteLog(logFile, "Unexpected packet received during image transfer.");
                    transferFailed = true;
                    break;
                }
            }

            if (!transferFailed && !receivedChunks.empty())
            {
                std::string outputFileName = "downloaded_" + requestedFile;

                if (FileChunkHelper::WriteChunksToFile(outputFileName, receivedChunks))
                {
                    std::cout << "Image saved as " << outputFileName << "\n";
                    Logger::WriteLog(logFile, "Image saved as " + outputFileName);
                }
                else
                {
                    std::cout << "Failed to save image.\n";
                    Logger::WriteLog(logFile, "Failed to save reconstructed image.");
                }
            }
            else if (!transferFailed)
            {
                std::cout << "No image chunks were received.\n";
                Logger::WriteLog(logFile, "No image chunks were received.");
            }
        }
        else if (choice == 5)
        {
            controller.Disconnect();
            std::cout << "Exiting client application.\n";
            Logger::WriteLog(logFile, "Client application exited.");
            Packet receivedPacket = controller.ReceivePacket();

            if (receivedPacket.command == CommandType::None && receivedPacket.payload.empty())
            {
                std::cout << "No packet received.\n";
                Logger::WriteLog(logFile, "No packet received from client.");
                continue;
            }

            std::cout << "Received Packet:\n";
            std::cout << "  Packet Type: " << receivedPacket.packetType << "\n";
            std::cout << "  Command: " << CommandToString(receivedPacket.command) << "\n";
            std::cout << "  Session ID: " << receivedPacket.sessionId << "\n";
            std::cout << "  Payload Length: " << receivedPacket.payloadLength << "\n";
            std::cout << "  Sequence Number: " << receivedPacket.sequenceNumber << "\n";
            std::cout << "  Payload: " << receivedPacket.payload << "\n";
            std::cout << "  Checksum: " << receivedPacket.checksum << "\n";

            Logger::WriteRxLog(logFile, "Received packet with command " + CommandToString(receivedPacket.command));

            Packet responsePacket = controller.ProcessPacket(receivedPacket);

            if (controller.SendPacket(responsePacket))
            {
                std::cout << "Sent Response Packet:\n";
                std::cout << "  Payload: " << responsePacket.payload << "\n";
                Logger::WriteTxLog(logFile, "Sent response packet with payload: " + responsePacket.payload);
            }
            else
            {
                std::cout << "Failed to send response packet.\n";
                Logger::WriteLog(logFile, "Failed to send response packet to client.");
            }
        }
        else if (choice == 4)
        {
            std::cout << "Current State: " << controller.GetCurrentState() << "\n";
            Logger::WriteLog(logFile, "Displayed current server state.");
        }
        else if (choice == 5)
        {
            controller.StopServer();
            std::cout << "Server stopped.\n";
            std::cout << "Current State: " << controller.GetCurrentState() << "\n";
            Logger::WriteLog(logFile, "Server stopped.");
        }
        else if (choice == 6)
        {
            controller.StopServer();
            std::cout << "Exiting server application.\n";
            Logger::WriteLog(logFile, "Server application exited.");
            break;
        }
        else
        {
            std::cout << "Invalid menu option.\n";
        }
    }

    return 0;
}