#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "BufferReader.h"
#include "ServerPacketHandler.h"

BYTE send_data[] = "Hell, World!";

class User : public PacketSession
{
public:
	~User()
	{
		puts("~User");
	}

	void OnConnectCompleted() override
	{
		//puts("Client OnConnectCompleted");

		Protocol::C_LOGIN pkt;
		auto send_buffer = ServerPacketHandler::MakeSendBuffer(pkt);
		Send(send_buffer);
	}

	void OnDisconnectCompleted() override
	{
		puts("Client OnDisconnectCompleted");
	}

	void OnRecvPacketProcessed(BYTE* buffer, int32 length) override
	{
		SharedPacketSession session = GetSharedPacketSession();
		ServerPacketHandler::HandlePacket(session, buffer, length);				
	}

	void OnSendCompleted(int32 length) override
	{
		//printf("OnSend len: %d\n", length);
	}
};

int main()
{
	this_thread::sleep_for(1s);

	ServerPacketHandler::Init();

	SharedClientService client_service = MakeShared<ClientService>(
	NetAddress(L"127.0.0.1", 9000),
	MakeShared<IocpCore>(),
	MakeShared<User>,		
	100);

	ASSERT_CRASH(client_service->Start());

	for (int32 i = 0; i < 2; ++i)
	{
		g_thread_manager->Launch([=]()
			{
				while (true)
				{
					client_service->GetIocpCore()->Dispatch();
				}
			});
	}

	// for chatting test
	Protocol::C_CHAT chat_pkt;
	chat_pkt.set_msg(u8"HELL WORLD");
	auto send_buffer = ServerPacketHandler::MakeSendBuffer(chat_pkt);
	while (true)
	{
		client_service->Broadcast(send_buffer);
		this_thread::sleep_for(1s);
	}

	g_thread_manager->Join();
}