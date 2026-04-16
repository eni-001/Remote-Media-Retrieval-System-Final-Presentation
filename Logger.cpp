#include "Logger.h"

#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace
{
    std::string GetTimestamp()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t nowTime = std::chrono::system_clock::to_time_t(now);

        std::tm localTime{};
#ifdef _WIN32
        localtime_s(&localTime, &nowTime);
#else
        localtime_r(&nowTime, &localTime);
#endif

        std::ostringstream timestamp;
        timestamp << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
        return timestamp.str();
    }

    void AppendLine(const std::string& filePath, const std::string& message)
    {
        std::ofstream outFile(filePath, std::ios::app);
        if (!outFile.is_open())
        {
            return;
        }

        outFile << GetTimestamp() << " - " << message << std::endl;
    }
}

void Logger::WriteLog(const std::string& filePath, const std::string& message)
{
    AppendLine(filePath, message);
}

void Logger::WriteTxLog(const std::string& filePath, const std::string& message)
{
    AppendLine(filePath, "[TX] " + message);
}

void Logger::WriteRxLog(const std::string& filePath, const std::string& message)
{
    AppendLine(filePath, "[RX] " + message);
}