#pragma once

#include <string>
#include "Packet.h"

class PacketFactory
{
public:
    static Packet CreateAuthPacket(const std::string& username, const std::string& password)
    {
        Packet packet;
        packet.packetType = 1;
        packet.command = CommandType::Authenticate;
        packet.sessionId = 0;
        packet.sequenceNumber = 0;
        packet.payload = username + "|" + password;
        packet.payloadLength = static_cast<int>(packet.payload.size());
        packet.checksum = "CHK_AUTH";
        return packet;
    }

    static Packet CreateListFilesPacket()
    {
        Packet packet;
        packet.packetType = 1;
        packet.command = CommandType::RequestFileList;
        packet.sessionId = 1;
        packet.sequenceNumber = 0;
        packet.payload = "LIST_FILES";
        packet.payloadLength = static_cast<int>(packet.payload.size());
        packet.checksum = "CHK_LIST";
        return packet;
    }

    static Packet CreateGetImagePacket(const std::string& fileName)
    {
        Packet packet;
        packet.packetType = 1;
        packet.command = CommandType::GetImage;
        packet.sessionId = 1;
        packet.sequenceNumber = 0;
        packet.payload = fileName;
        packet.payloadLength = static_cast<int>(packet.payload.size());
        packet.checksum = "CHK_IMG";
        return packet;
    }

    static Packet CreateResponsePacket(CommandType command, const std::string& payload, int sessionId = 1)
    {
        Packet packet;
        packet.packetType = 2;
        packet.command = command;
        packet.sessionId = sessionId;
        packet.sequenceNumber = 0;
        packet.payload = payload;
        packet.payloadLength = static_cast<int>(packet.payload.size());
        packet.checksum = "CHK_RESP";
        return packet;
    }

    static Packet CreateTransferChunkPacket(const std::string& chunkData, int sequenceNumber, int sessionId = 1)
    {
        Packet packet;
        packet.packetType = 3;
        packet.command = CommandType::TransferChunk;
        packet.sessionId = sessionId;
        packet.sequenceNumber = sequenceNumber;
        packet.payload = chunkData;
        packet.payloadLength = static_cast<int>(packet.payload.size());
        packet.checksum = "CHK_CHUNK";
        return packet;
    }

    static Packet CreateTransferCompletePacket(int sessionId = 1)
    {
        Packet packet;
        packet.packetType = 4;
        packet.command = CommandType::TransferComplete;
        packet.sessionId = sessionId;
        packet.sequenceNumber = -1;
        packet.payload = "TRANSFER_COMPLETE";
        packet.payloadLength = static_cast<int>(packet.payload.size());
        packet.checksum = "CHK_DONE";
        return packet;
    }

    static Packet CreateErrorPacket(const std::string& message, int sessionId = 1)
    {
        Packet packet;
        packet.packetType = 5;
        packet.command = CommandType::Error;
        packet.sessionId = sessionId;
        packet.sequenceNumber = 0;
        packet.payload = message;
        packet.payloadLength = static_cast<int>(packet.payload.size());
        packet.checksum = "CHK_ERR";
        return packet;
    }
};