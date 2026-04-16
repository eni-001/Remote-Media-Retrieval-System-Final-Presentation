#pragma once

#include <string>

class Logger
{
public:
    static void WriteLog(const std::string& filePath, const std::string& message);
    static void WriteTxLog(const std::string& filePath, const std::string& message);
    static void WriteRxLog(const std::string& filePath, const std::string& message);
};