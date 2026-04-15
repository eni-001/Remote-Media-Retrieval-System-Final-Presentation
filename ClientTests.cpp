#include "pch.h"
#include "CppUnitTest.h"

#include "ClientController.h"
#include "Packet.h"
#include "PacketFactory.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTests
{
    TEST_CLASS(ClientTests)
    {
    public:

        TEST_METHOD(Client_Default_IsNotConnected)
        {
            ClientController c;
            Assert::IsFalse(c.IsConnected());
        }

        TEST_METHOD(Client_Default_IsNotAuthenticated)
        {
            ClientController c;
            Assert::IsFalse(c.IsAuthenticated());
        }

        TEST_METHOD(Client_SetAuthenticated_TrueWorks)
        {
            ClientController c;
            c.SetAuthenticated(true);
            Assert::IsTrue(c.IsAuthenticated());
        }

        TEST_METHOD(Client_SetAuthenticated_FalseWorks)
        {
            ClientController c;
            c.SetAuthenticated(true);
            c.SetAuthenticated(false);
            Assert::IsFalse(c.IsAuthenticated());
        }

        TEST_METHOD(Client_Disconnect_ResetsAuthentication)
        {
            ClientController c;
            c.SetAuthenticated(true);
            c.Disconnect();
            Assert::IsFalse(c.IsAuthenticated());
        }

        TEST_METHOD(Client_Disconnect_LeavesDisconnectedState)
        {
            ClientController c;
            c.Disconnect();
            Assert::IsFalse(c.IsConnected());
        }

        TEST_METHOD(Client_ConnectToServer_EmptyIp_ReturnsFalse)
        {
            ClientController c;
            Assert::IsFalse(c.ConnectToServer("", "8080"));
        }

        TEST_METHOD(Client_ConnectToServer_EmptyPort_ReturnsFalse)
        {
            ClientController c;
            Assert::IsFalse(c.ConnectToServer("127.0.0.1", ""));
        }

        TEST_METHOD(Client_ConnectToServer_NonNumericPort_ReturnsFalse)
        {
            ClientController c;
            Assert::IsFalse(c.ConnectToServer("127.0.0.1", "abc"));
        }

        TEST_METHOD(Client_ConnectToServer_ZeroPort_ReturnsFalse)
        {
            ClientController c;
            Assert::IsFalse(c.ConnectToServer("127.0.0.1", "0"));
        }

        TEST_METHOD(Client_ConnectToServer_NegativePort_ReturnsFalse)
        {
            ClientController c;
            Assert::IsFalse(c.ConnectToServer("127.0.0.1", "-1"));
        }

        TEST_METHOD(Client_Authenticate_WithoutConnection_ReturnsFalse)
        {
            ClientController c;
            Assert::IsFalse(c.Authenticate("eni", "1234"));
        }

        TEST_METHOD(Client_Authenticate_EmptyUsername_ReturnsFalse)
        {
            ClientController c;
            Assert::IsFalse(c.Authenticate("", "1234"));
        }

        TEST_METHOD(Client_Authenticate_EmptyPassword_ReturnsFalse)
        {
            ClientController c;
            Assert::IsFalse(c.Authenticate("eni", ""));
        }

        TEST_METHOD(Client_SendPacket_WithoutConnection_ReturnsFalse)
        {
            ClientController c;
            Packet p = PacketFactory::CreateListFilesPacket();
            Assert::IsFalse(c.SendPacket(p));
        }

        TEST_METHOD(Client_ReceivePacket_WithoutConnection_ReturnsDefaultPacket)
        {
            ClientController c;
            Packet p = c.ReceivePacket();
            Assert::AreEqual(0, p.packetType);
            Assert::AreEqual(static_cast<int>(CommandType::None), static_cast<int>(p.command));
        }
    };
}