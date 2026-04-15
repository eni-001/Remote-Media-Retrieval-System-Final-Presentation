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
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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
            }
            else
            {
                std::cout << "Authentication failed.\n";
                Logger::WriteLog(logFile, "Client authentication failed.");
            }
        }
        else if (choice == 3)
        {
            std::vector<FileRecord> files = controller.GetMockFileList();

            if (files.empty())
            {
                std::cout << "No files available or client not authenticated.\n";
                Logger::WriteLog(logFile, "File list request failed or no files returned.");
            }
            else
            {
                std::cout << "\nAvailable Files:\n";
                for (size_t i = 0; i < files.size(); ++i)
                {
                    std::cout << i + 1 << ". "
                        << files[i].fileName
                        << " | Type: " << files[i].fileType
                        << " | Size: " << files[i].fileSizeBytes << " bytes\n";
                }
                Logger::WriteRxLog(logFile, "File list retrieved successfully.");
            }
        }
        else if (choice == 4)
        {
            std::vector<FileRecord> files = controller.GetMockFileList();

            if (files.empty())
            {
                std::cout << "No files available or client not authenticated.\n";
                Logger::WriteLog(logFile, "Get image failed: file list unavailable.");
                continue;
            }

            std::cout << "\nAvailable Files:\n";
            for (size_t i = 0; i < files.size(); ++i)
            {
                std::cout << i + 1 << ". "
                    << files[i].fileName
                    << " | Type: " << files[i].fileType
                    << " | Size: " << files[i].fileSizeBytes << " bytes\n";
            }

            std::cout << "Select file number: ";
            size_t fileChoice = 0;

            if (!(std::cin >> fileChoice))
            {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid file selection.\n";
                Logger::WriteLog(logFile, "Get image failed: invalid selection input.");
                continue;
            }

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (fileChoice < 1 || fileChoice > files.size())
            {
                std::cout << "Invalid file selection.\n";
                Logger::WriteLog(logFile, "Get image failed: selection out of range.");
                continue;
            }

            FileRecord selectedFile = files[fileChoice - 1];
            std::cout << "Requested image: " << selectedFile.fileName << '\n';
            Logger::WriteTxLog(logFile, "Requested image: " + selectedFile.fileName);
        }
        else if (choice == 5)
        {
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