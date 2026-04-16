#include <iostream>
#include <string>
#include <limits>

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