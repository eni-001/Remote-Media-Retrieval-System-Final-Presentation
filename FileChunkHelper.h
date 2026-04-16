#pragma once

#include <string>
#include <vector>

class FileChunkHelper
{
public:
    static std::vector<std::string> ReadFileInChunks(const std::string& filePath, std::size_t chunkSize);
    static bool WriteChunksToFile(const std::string& filePath, const std::vector<std::string>& chunks);
};