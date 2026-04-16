#include "pch.h"
#include "CppUnitTest.h"

#include <filesystem>
#include <fstream>

#include "ServerStateMachine.h"
#include "ServerController.h"
#include "PacketFactory.h"
#include "Packet.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
namespace fs = std::filesystem;

namespace
{
    class ScopedTempCurrentPath
    {
    private:
        fs::path oldPath;
        fs::path tempPath;

    public:
        ScopedTempCurrentPath(const std::string& folderName)
        {
            oldPath = fs::current_path();
            tempPath = fs::temp_directory_path() / folderName;

            if (fs::exists(tempPath))
            {
                fs::remove_all(tempPath);
            }

            fs::create_directories(tempPath);
            fs::current_path(tempPath);
        }

        ~ScopedTempCurrentPath()
        {
            fs::current_path(oldPath);
            if (fs::exists(tempPath))
            {
                fs::remove_all(tempPath);
            }
        }

        fs::path GetTempPath() const
        {
            return tempPath;
        }
    };
}

namespace UnitTests
{
    TEST_CLASS(ServerTests)
    {
    public:

        TEST_METHOD(ServerStateMachine_DefaultState_IsStartup)
        {
            ServerStateMachine sm;
            Assert::AreEqual(static_cast<int>(ServerState::Startup), static_cast<int>(sm.GetCurrentState()));
        }

        TEST_METHOD(ServerStateMachine_SetWaitingForAuth_Works)
        {
            ServerStateMachine sm;
            sm.SetState(ServerState::WaitingForAuth);
            Assert::AreEqual(static_cast<int>(ServerState::WaitingForAuth), static_cast<int>(sm.GetCurrentState()));
        }

        TEST_METHOD(ServerStateMachine_SetAuthenticatedReady_Works)
        {
            ServerStateMachine sm;
            sm.SetState(ServerState::AuthenticatedReady);
            Assert::AreEqual(static_cast<int>(ServerState::AuthenticatedReady), static_cast<int>(sm.GetCurrentState()));
        }

        TEST_METHOD(ServerStateMachine_SetTransferring_Works)
        {
            ServerStateMachine sm;
            sm.SetState(ServerState::Transferring);
            Assert::AreEqual(static_cast<int>(ServerState::Transferring), static_cast<int>(sm.GetCurrentState()));
        }

        TEST_METHOD(ServerStateMachine_SetError_Works)
        {
            ServerStateMachine sm;
            sm.SetState(ServerState::Error);
            Assert::AreEqual(static_cast<int>(ServerState::Error), static_cast<int>(sm.GetCurrentState()));
        }

        TEST_METHOD(ServerController_Default_IsNotRunning)
        {
            ServerController s;
            Assert::IsFalse(s.IsRunning());
        }

        TEST_METHOD(ServerController_DefaultState_IsStartup)
        {
            ServerController s;
            Assert::IsTrue(s.GetCurrentState() == "STARTUP");
        }

        TEST_METHOD(ServerController_StartServer_EmptyPort_ReturnsFalse)
        {
            ServerController s;
            Assert::IsFalse(s.StartServer(""));
        }

        TEST_METHOD(ServerController_StartServer_ZeroPort_ReturnsFalse)
        {
            ServerController s;
            Assert::IsFalse(s.StartServer("0"));
        }

        TEST_METHOD(ServerController_ReceivePacket_WithoutClient_ReturnsDefaultPacket)
        {
            ServerController s;
            Packet p = s.ReceivePacket();
            Assert::AreEqual(0, p.packetType);
            Assert::AreEqual(static_cast<int>(CommandType::None), static_cast<int>(p.command));
        }

        TEST_METHOD(ServerController_ProcessAuthPacket_ReturnsAuthOk)
        {
            ServerController s;
            Packet auth = PacketFactory::CreateAuthPacket("eni", "1234");

            Packet response = s.ProcessPacket(auth);

            Assert::AreEqual(static_cast<int>(CommandType::Response), static_cast<int>(response.command));
            Assert::IsTrue(response.payload == "AUTH_OK");
        }

        TEST_METHOD(ServerController_ProcessAuthPacket_SetsAuthenticatedReadyState)
        {
            ServerController s;
            Packet auth = PacketFactory::CreateAuthPacket("eni", "1234");

            s.ProcessPacket(auth);

            Assert::IsTrue(s.GetCurrentState() == "AUTHENTICATED_READY");
        }

        TEST_METHOD(ServerController_ProcessFileList_WithoutAuth_ReturnsError)
        {
            ServerController s;
            Packet list = PacketFactory::CreateListFilesPacket();

            Packet response = s.ProcessPacket(list);

            Assert::AreEqual(static_cast<int>(CommandType::Error), static_cast<int>(response.command));
            Assert::IsTrue(response.payload == "ERROR|NOT_AUTHENTICATED" || response.payload == "ERROR|NO_FILES_AVAILABLE");
        }

        TEST_METHOD(ServerController_ProcessFileList_AfterAuth_ReturnsResponsePacket)
        {
            ScopedTempCurrentPath temp("RMRS_ServerTests_FileList1");
            fs::create_directories(fs::current_path() / "ServerFiles");

            std::ofstream(fs::current_path() / "ServerFiles" / "image1.jpg", std::ios::binary) << "abc";

            ServerController s;
            s.ProcessPacket(PacketFactory::CreateAuthPacket("eni", "1234"));

            Packet response = s.ProcessPacket(PacketFactory::CreateListFilesPacket());

            Assert::AreEqual(static_cast<int>(CommandType::Response), static_cast<int>(response.command));
        }

        TEST_METHOD(ServerController_ProcessFileList_AfterAuth_ContainsExistingFile)
        {
            ScopedTempCurrentPath temp("RMRS_ServerTests_FileList2");
            fs::create_directories(fs::current_path() / "ServerFiles");

            std::ofstream(fs::current_path() / "ServerFiles" / "image1.jpg", std::ios::binary) << "abc";
            std::ofstream(fs::current_path() / "ServerFiles" / "image2.jpg", std::ios::binary) << "def";

            ServerController s;
            s.ProcessPacket(PacketFactory::CreateAuthPacket("eni", "1234"));

            Packet response = s.ProcessPacket(PacketFactory::CreateListFilesPacket());

            Assert::IsTrue(response.payload.find("image1.jpg") != std::string::npos);
            Assert::IsTrue(response.payload.find("image2.jpg") != std::string::npos);
        }

        TEST_METHOD(ServerController_ProcessGetImage_WithoutAuth_ReturnsError)
        {
            ServerController s;
            Packet getImage = PacketFactory::CreateGetImagePacket("image1.jpg");

            Packet response = s.ProcessPacket(getImage);

            Assert::AreEqual(static_cast<int>(CommandType::Error), static_cast<int>(response.command));
            Assert::IsTrue(response.payload == "ERROR|NOT_AUTHENTICATED");
        }

        TEST_METHOD(ServerController_ProcessInvalidCommand_ReturnsInvalidCommandError)
        {
            ServerController s;
            Packet p;
            p.packetType = 1;
            p.command = CommandType::None;
            p.sessionId = 1;
            p.payload = "";
            p.payloadLength = 0;
            p.sequenceNumber = 0;
            p.checksum = "CHK";

            Packet response = s.ProcessPacket(p);

            Assert::AreEqual(static_cast<int>(CommandType::Error), static_cast<int>(response.command));
            Assert::IsTrue(response.payload == "ERROR|INVALID_COMMAND");
        }

        TEST_METHOD(ServerController_ProcessInvalidPacket_ReturnsInvalidPacketError)
        {
            ServerController s;
            Packet p;
            p.packetType = 1;
            p.command = CommandType::Authenticate;
            p.sessionId = 1;
            p.payload = "abc";
            p.payloadLength = 99;
            p.sequenceNumber = 0;
            p.checksum = "CHK";

            Packet response = s.ProcessPacket(p);

            Assert::AreEqual(static_cast<int>(CommandType::Error), static_cast<int>(response.command));
            Assert::IsTrue(response.payload == "ERROR|INVALID_PACKET");
        }
    };
}