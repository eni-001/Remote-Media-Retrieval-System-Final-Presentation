#include <iostream>
#include <string>
#include <vector>
#include <limits>

#include "ClientController.h"
#include "Logger.h"
#include "FileRecord.h"

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

            if (controller.Authenticate(username, password))
            {
                std::cout << "Authentication successful.\n";
                Logger::WriteTxLog(logFile, "Client authenticated successfully.");
                Logger::WriteRxLog(logFile, "Server returned AUTH_OK.");
            }
            else
            {
                std::cout << "Authentication failed.\n";
                Logger::WriteLog(logFile, "Client authentication failed.");
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

            if (!controller.SendRawMessage("LIST_FILES"))
            {
                std::cout << "Failed to send file list request.\n";
                Logger::WriteLog(logFile, "Failed to send LIST_FILES request.");
                continue;
            }

            Logger::WriteTxLog(logFile, "Sent LIST_FILES request.");

            std::string response = controller.ReceiveRawMessage();
            if (response.empty())
            {
                std::cout << "No response from server.\n";
                Logger::WriteLog(logFile, "No response received for LIST_FILES.");
                continue;
            }

            std::cout << "\nServer Response:\n" << response << "\n";
            Logger::WriteRxLog(logFile, "Received file list response from server.");
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

            std::string requestMessage = "GET_IMAGE|" + requestedFile;

            if (!controller.SendRawMessage(requestMessage))
            {
                std::cout << "Failed to send image request.\n";
                Logger::WriteLog(logFile, "Failed to send GET_IMAGE request.");
                continue;
            }

            Logger::WriteTxLog(logFile, "Requested image: " + requestedFile);

            std::string response = controller.ReceiveRawMessage();
            if (response.empty())
            {
                std::cout << "No response from server.\n";
                Logger::WriteLog(logFile, "No response received for GET_IMAGE.");
                continue;
            }

            std::cout << "Server Response: " << response << "\n";
            Logger::WriteRxLog(logFile, "Received GET_IMAGE response from server.");
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