#include <iostream>
#include <string>
#include <limits>

#include "ServerController.h"
#include "Logger.h"

void DisplayServerMenu()
{
    std::cout << "\n===== Server Application Menu =====\n";
    std::cout << "1. Start Server\n";
    std::cout << "2. Authenticate Client\n";
    std::cout << "3. Begin Transfer\n";
    std::cout << "4. Show Current State\n";
    std::cout << "5. Stop Server\n";
    std::cout << "6. Exit\n";
    std::cout << "Select an option: ";
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
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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
            std::string username;
            std::string password;

            std::cout << "Enter Client Username: ";
            std::getline(std::cin, username);

            std::cout << "Enter Client Password: ";
            std::getline(std::cin, password);

            if (controller.AuthenticateClient(username, password))
            {
                std::cout << "Client authenticated successfully.\n";
                std::cout << "Current State: " << controller.GetCurrentState() << "\n";
                Logger::WriteLog(logFile, "Client authenticated successfully.");
            }
            else
            {
                std::cout << "Client authentication failed.\n";
                std::cout << "Current State: " << controller.GetCurrentState() << "\n";
                Logger::WriteLog(logFile, "Client authentication failed.");
            }
        }
        else if (choice == 3)
        {
            if (controller.BeginTransfer())
            {
                std::cout << "Transfer started successfully.\n";
                std::cout << "Current State: " << controller.GetCurrentState() << "\n";
                Logger::WriteLog(logFile, "Transfer started.");
            }
            else
            {
                std::cout << "Transfer could not start.\n";
                std::cout << "Current State: " << controller.GetCurrentState() << "\n";
                Logger::WriteLog(logFile, "Transfer failed to start.");
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