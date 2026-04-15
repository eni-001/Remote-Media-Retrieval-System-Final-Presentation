#include <iostream>
#include <string>
#include <limits>

#include "ClientController.h"
#include "Logger.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "CommandType.h"

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

int main()
{
    ClientController controller;
    const std::string logFile = "client_log.txt";

    int choice = 0;
    bool authenticated = false;

    std::cout << "Remote Media Retrieval System - Client Application\n";

    while (true)
    {
        DisplayMenu();

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
                authenticated = true;
                std::cout << "Authentication successful.\n";
                Logger::WriteRxLog(logFile, "Received AUTH_OK packet.");
            }
            else
            {
                authenticated = false;
                std::cout << "Authentication failed.\n";
                Logger::WriteLog(logFile, "Authentication response packet invalid.");
            }
        }
        else if (choice == 3)
        {
            if (!authenticated)
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

            std::cout << "\nServer Response:\n" << responsePacket.payload << "\n";
            Logger::WriteRxLog(logFile, "Received file list response packet.");
        }
        else if (choice == 4)
        {
            if (!authenticated)
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

            Packet responsePacket = controller.ReceivePacket();
            if (responsePacket.payload.empty())
            {
                std::cout << "No response from server.\n";
                Logger::WriteLog(logFile, "No response packet received for GET_IMAGE.");
                continue;
            }

            std::cout << "Server Response: " << responsePacket.payload << "\n";
            Logger::WriteRxLog(logFile, "Received GET_IMAGE response packet.");
        }
        else if (choice == 5)
        {
            controller.Disconnect();
            std::cout << "Exiting client application.\n";
            Logger::WriteLog(logFile, "Client application exited.");
            break;
        }
        else
        {
            std::cout << "Invalid menu option.\n";
        }
    }

    return 0;
}