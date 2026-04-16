#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>

#include "CommandType.h"

struct Packet
{
    int packetType;
    CommandType command;
    int sessionId;
    int payloadLength;
    int sequenceNumber;
    std::string payload;
    std::string checksum;

    Packet()
        : packetType(0),
        command(CommandType::None),
        sessionId(0),
        payloadLength(0),
        sequenceNumber(0),
        payload(""),
        checksum("")
    {
    }

    std::string Serialize() const
    {
        std::ostringstream out;
        out << packetType << ";"
            << static_cast<int>(command) << ";"
            << sessionId << ";"
            << payloadLength << ";"
            << sequenceNumber << ";"
            << payload << ";"
            << checksum;
        return out.str();
    }

    static bool TryParseInt(const std::string& text, int& value)
    {
        try
        {
            size_t idx = 0;
            int parsed = std::stoi(text, &idx);
            if (idx != text.size())
            {
                return false;
            }
            value = parsed;
            return true;
        }
        catch (...)
        {
            return false;
        }
    }

    static Packet Deserialize(const std::string& data)
    {
        Packet packet;
        std::istringstream in(data);
        std::string token;
        std::vector<std::string> fields;

        while (std::getline(in, token, ';'))
        {
            fields.push_back(token);
        }

        if (fields.size() < 7)
        {
            return Packet();
        }

        int parsedValue = 0;

        if (!TryParseInt(fields[0], parsedValue))
        {
            return Packet();
        }
        packet.packetType = parsedValue;

        if (!TryParseInt(fields[1], parsedValue))
        {
            return Packet();
        }
        packet.command = static_cast<CommandType>(parsedValue);

        if (!TryParseInt(fields[2], parsedValue))
        {
            return Packet();
        }
        packet.sessionId = parsedValue;

        if (!TryParseInt(fields[3], parsedValue))
        {
            return Packet();
        }
        packet.payloadLength = parsedValue;

        if (!TryParseInt(fields[4], parsedValue))
        {
            return Packet();
        }
        packet.sequenceNumber = parsedValue;

        packet.payload = fields[5];
        packet.checksum = fields[6];

        return packet;
    }

    bool IsValid() const
    {
        if (packetType < 0)
        {
            return false;
        }

        if (payloadLength != static_cast<int>(payload.size()))
        {
            return false;
        }

        return true;
    }
};