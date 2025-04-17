#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "ServerPacketHandler.h"
#include "ServerSession.h"

enum WorkerInfo
{
	// TODO : Job 처리가 충분한 시간이 아닐 때 자동 보정
	TICK = 64,
};

void DoWorkerJob(SharedClientService& service)
{
	while (true)
	{
		// 네트워크 입출력 처리 + 패킷 핸들
		service->GetIocpCore()->Dispatch(10);

		ThreadManager::DistributeReservedJobs();

		// Job
		tls_end_tick_count = GetTickCount64() + WorkerInfo::TICK;
		ThreadManager::WorkGlobalQueue();
	}
}

int main()
{
	this_thread::sleep_for(1s);

	ServerPacketHandler::Init();

	SharedClientService client_service = MakeShared<ClientService>(
	NetAddress(L"127.0.0.1", 9999),
	MakeShared<IocpCore>(),
	MakeShared<ServerSession>,
	50);

	ASSERT_CRASH(client_service->Start());

	for (int32 i = 0; i < 6; ++i)
	{
		g_thread_manager->Launch([&client_service]()
			{
				DoWorkerJob(client_service);
			});
	}

	g_thread_manager->Join();
}