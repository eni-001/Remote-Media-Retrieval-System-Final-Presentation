#include "pch.h"
#include "CppUnitTest.h"

#include <string>
#include <vector>
#include <filesystem>

#include "Packet.h"
#include "PacketFactory.h"
#include "FileChunkHelper.h"
#include "CommandType.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace fs = std::filesystem;

namespace UnitTests
{
    TEST_CLASS(SharedLibTests)
    {
    public:

        TEST_METHOD(Packet_DefaultConstructor_InitializesFields)
        {
            Packet p;
            Assert::AreEqual(0, p.packetType);
            Assert::AreEqual(static_cast<int>(CommandType::None), static_cast<int>(p.command));
            Assert::AreEqual(0, p.sessionId);
            Assert::AreEqual(0, p.payloadLength);
            Assert::AreEqual(0, p.sequenceNumber);
            Assert::IsTrue(p.payload.empty());
            Assert::IsTrue(p.checksum.empty());
        }

        TEST_METHOD(Packet_Serialize_ReturnsNonEmptyString)
        {
            Packet p;
            p.packetType = 1;
            p.command = CommandType::Authenticate;
            p.sessionId = 1;
            p.payload = "abc";
            p.payloadLength = 3;
            p.sequenceNumber = 0;
            p.checksum = "CHK";

            std::string s = p.Serialize();
            Assert::IsFalse(s.empty());
        }

        TEST_METHOD(Packet_Deserialize_ParsesPacketType)
        {
            Packet p = Packet::Deserialize("1;2;3;5;5;hello;CHK");
            Assert::AreEqual(1, p.packetType);
        }

        TEST_METHOD(Packet_Deserialize_ParsesCommand)
        {
            Packet p = Packet::Deserialize("1;2;3;5;5;hello;CHK");
            Assert::AreEqual(static_cast<int>(CommandType::Authenticate), static_cast<int>(p.command));
        }

        TEST_METHOD(Packet_Deserialize_ParsesSessionId)
        {
            Packet p = Packet::Deserialize("1;2;3;5;5;hello;CHK");
            Assert::AreEqual(3, p.sessionId);
        }

        TEST_METHOD(Packet_Deserialize_ParsesPayloadLength)
        {
            Packet p = Packet::Deserialize("1;2;3;5;5;hello;CHK");
            Assert::AreEqual(5, p.payloadLength);
        }

        TEST_METHOD(Packet_Deserialize_ParsesSequenceNumber)
        {
            Packet p = Packet::Deserialize("1;2;3;5;5;hello;CHK");
            Assert::AreEqual(5, p.sequenceNumber);
        }

        TEST_METHOD(Packet_Deserialize_ParsesPayload)
        {
            Packet p = Packet::Deserialize("1;2;3;5;5;hello;CHK");
            Assert::IsTrue(p.payload == "hello");
        }

        TEST_METHOD(Packet_Deserialize_ParsesChecksum)
        {
            Packet p = Packet::Deserialize("1;2;3;5;5;hello;CHK");
            Assert::IsTrue(p.checksum == "CHK");
        }

        TEST_METHOD(Packet_Deserialize_InvalidFieldCount_ReturnsDefaultPacket)
        {
            Packet p = Packet::Deserialize("1;2;3");
            Assert::AreEqual(0, p.packetType);
            Assert::AreEqual(static_cast<int>(CommandType::None), static_cast<int>(p.command));
        }

        TEST_METHOD(Packet_Deserialize_InvalidInteger_ReturnsDefaultPacket)
        {
            Packet p = Packet::Deserialize("abc;2;3;4;5;hello;CHK");
            Assert::AreEqual(0, p.packetType);
            Assert::AreEqual(static_cast<int>(CommandType::None), static_cast<int>(p.command));
        }

        TEST_METHOD(Packet_TryParseInt_ValidNumber_ReturnsTrue)
        {
            int value = 0;
            bool ok = Packet::TryParseInt("123", value);
            Assert::IsTrue(ok);
            Assert::AreEqual(123, value);
        }

        TEST_METHOD(Packet_TryParseInt_Alpha_ReturnsFalse)
        {
            int value = 0;
            bool ok = Packet::TryParseInt("abc", value);
            Assert::IsFalse(ok);
        }

        TEST_METHOD(Packet_TryParseInt_PartialNumber_ReturnsFalse)
        {
            int value = 0;
            bool ok = Packet::TryParseInt("12x", value);
            Assert::IsFalse(ok);
        }

        TEST_METHOD(Packet_IsValid_TrueWhenPayloadLengthMatches)
        {
            Packet p;
            p.packetType = 1;
            p.payload = "hello";
            p.payloadLength = 5;
            Assert::IsTrue(p.IsValid());
        }

        TEST_METHOD(Packet_IsValid_FalseWhenPayloadLengthDoesNotMatch)
        {
            Packet p;
            p.packetType = 1;
            p.payload = "hello";
            p.payloadLength = 4;
            Assert::IsFalse(p.IsValid());
        }

        TEST_METHOD(PacketFactory_CreateAuthPacket_SetsFieldsCorrectly)
        {
            Packet p = PacketFactory::CreateAuthPacket("eni", "1234");
            Assert::AreEqual(1, p.packetType);
            Assert::AreEqual(static_cast<int>(CommandType::Authenticate), static_cast<int>(p.command));
            Assert::IsTrue(p.payload == "eni|1234");
            Assert::AreEqual(static_cast<int>(p.payload.size()), p.payloadLength);
        }

        TEST_METHOD(PacketFactory_CreateListFilesPacket_SetsFieldsCorrectly)
        {
            Packet p = PacketFactory::CreateListFilesPacket();
            Assert::AreEqual(static_cast<int>(CommandType::RequestFileList), static_cast<int>(p.command));
            Assert::IsTrue(p.payload == "LIST_FILES");
        }

        TEST_METHOD(PacketFactory_CreateGetImagePacket_SetsFieldsCorrectly)
        {
            Packet p = PacketFactory::CreateGetImagePacket("image1.jpg");
            Assert::AreEqual(static_cast<int>(CommandType::GetImage), static_cast<int>(p.command));
            Assert::IsTrue(p.payload == "image1.jpg");
        }

        TEST_METHOD(PacketFactory_CreateResponsePacket_SetsFieldsCorrectly)
        {
            Packet p = PacketFactory::CreateResponsePacket(CommandType::Response, "AUTH_OK", 1);
            Assert::AreEqual(2, p.packetType);
            Assert::AreEqual(static_cast<int>(CommandType::Response), static_cast<int>(p.command));
            Assert::IsTrue(p.payload == "AUTH_OK");
        }

        TEST_METHOD(PacketFactory_CreateTransferChunkPacket_SetsFieldsCorrectly)
        {
            Packet p = PacketFactory::CreateTransferChunkPacket("ABCDEF", 7, 1);
            Assert::AreEqual(3, p.packetType);
            Assert::AreEqual(static_cast<int>(CommandType::TransferChunk), static_cast<int>(p.command));
            Assert::AreEqual(7, p.sequenceNumber);
            Assert::IsTrue(p.payload == "ABCDEF");
        }

        TEST_METHOD(PacketFactory_CreateTransferCompletePacket_SetsFieldsCorrectly)
        {
            Packet p = PacketFactory::CreateTransferCompletePacket(1);
            Assert::AreEqual(4, p.packetType);
            Assert::AreEqual(static_cast<int>(CommandType::TransferComplete), static_cast<int>(p.command));
            Assert::AreEqual(-1, p.sequenceNumber);
            Assert::IsTrue(p.payload == "TRANSFER_COMPLETE");
        }

        TEST_METHOD(PacketFactory_CreateErrorPacket_SetsFieldsCorrectly)
        {
            Packet p = PacketFactory::CreateErrorPacket("ERROR|BAD", 1);
            Assert::AreEqual(5, p.packetType);
            Assert::AreEqual(static_cast<int>(CommandType::Error), static_cast<int>(p.command));
            Assert::IsTrue(p.payload == "ERROR|BAD");
        }

        TEST_METHOD(FileChunkHelper_ReadMissingFile_ReturnsEmptyVector)
        {
            std::vector<std::string> chunks = FileChunkHelper::ReadFileInChunks("file_that_does_not_exist.bin", 8);
            Assert::AreEqual(static_cast<size_t>(0), chunks.size());
        }

        TEST_METHOD(FileChunkHelper_WriteChunksToFile_CreatesFile)
        {
            fs::path tempFile = fs::temp_directory_path() / "rmrs_test_write.bin";
            std::vector<std::string> chunks = { "abc", "def" };

            bool ok = FileChunkHelper::WriteChunksToFile(tempFile.string(), chunks);

            Assert::IsTrue(ok);
            Assert::IsTrue(fs::exists(tempFile));

            if (fs::exists(tempFile))
            {
                fs::remove(tempFile);
            }
        }

        TEST_METHOD(FileChunkHelper_WriteThenRead_RoundTripMatchesContent)
        {
            fs::path tempFile = fs::temp_directory_path() / "rmrs_test_roundtrip.bin";
            std::vector<std::string> chunksToWrite = { "hello", "world", "12345" };

            bool writeOk = FileChunkHelper::WriteChunksToFile(tempFile.string(), chunksToWrite);
            Assert::IsTrue(writeOk);

            std::vector<std::string> chunksRead = FileChunkHelper::ReadFileInChunks(tempFile.string(), 5);

            std::string written;
            for (const auto& c : chunksToWrite) written += c;

            std::string readBack;
            for (const auto& c : chunksRead) readBack += c;

            Assert::IsTrue(written == readBack);

            if (fs::exists(tempFile))
            {
                fs::remove(tempFile);
            }
        }

        TEST_METHOD(FileChunkHelper_ReadFileInChunks_SplitsIntoMultipleChunks)
        {
            fs::path tempFile = fs::temp_directory_path() / "rmrs_test_chunks.bin";
            std::vector<std::string> chunksToWrite = { "abcdefghij" };

            bool writeOk = FileChunkHelper::WriteChunksToFile(tempFile.string(), chunksToWrite);
            Assert::IsTrue(writeOk);

            std::vector<std::string> chunksRead = FileChunkHelper::ReadFileInChunks(tempFile.string(), 4);
            Assert::IsTrue(chunksRead.size() >= 3);

            if (fs::exists(tempFile))
            {
                fs::remove(tempFile);
            }
        }
    };
}