#include "FileChunkHelper.h"

#include <fstream>
#include <vector>
#include <string>

std::vector<std::string> FileChunkHelper::ReadFileInChunks(const std::string& filePath, std::size_t chunkSize)
{
    std::vector<std::string> chunks;

    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile.is_open())
    {
        return chunks;
    }

    while (true)
    {
        std::vector<char> buffer(chunkSize);
        inFile.read(buffer.data(), static_cast<std::streamsize>(chunkSize));
        std::streamsize bytesRead = inFile.gcount();

        if (bytesRead <= 0)
        {
            break;
        }

        chunks.emplace_back(buffer.data(), static_cast<std::size_t>(bytesRead));
    }

    inFile.close();
    return chunks;
}

bool FileChunkHelper::WriteChunksToFile(const std::string& filePath, const std::vector<std::string>& chunks)
{
    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile.is_open())
    {
        return false;
    }

    for (const auto& chunk : chunks)
    {
        outFile.write(chunk.data(), static_cast<std::streamsize>(chunk.size()));
    }

    outFile.close();
    return true;
}