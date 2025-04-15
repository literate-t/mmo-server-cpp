#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "ServerPacketHandler.h"
#include "ServerSession.h"

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

	g_thread_manager->Join();
}