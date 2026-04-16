#pragma once

#include <string>
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
};