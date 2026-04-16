#pragma once

#include <string>

struct FileRecord
{
    std::string fileName;
    std::string fileType;
    long long fileSizeBytes;

    FileRecord()
        : fileName(""),
        fileType(""),
        fileSizeBytes(0)
    {
    }

    FileRecord(const std::string& name, const std::string& type, long long size)
        : fileName(name),
        fileType(type),
        fileSizeBytes(size)
    {
    }
};