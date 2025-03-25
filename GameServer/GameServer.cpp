#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "ListenHandler.h"
#include "NetAddress.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ClientPacketHandler.h"
#include <tchar.h>
#include "Protocol.pb.h"
#include "Job.h"
#include "Room.h"
#include "ServerDB.h"
#include "ConfigManager.h"
#include "DataManager.h"
#include "RoomManager.h"

enum WorkerInfo
{
	// TODO : Job 처리가 충분한 시간이 아닐 때 자동 보정
	TICK = 64,
};

void DoWorkerJob(SharedServerService& service)
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
	ServerDB server_db;
	server_db.MakeTables();
	ClientPacketHandler::Init();
	g_room_manager->Add(1);

	SharedServerService _server_service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 9999),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>,
		MakeShared<ListenHandler>(),
		100);

	ASSERT_CRASH(_server_service->Start());

	for (int32 i = 0; i < 5; ++i)
	{
		g_thread_manager->Launch([&_server_service]()
			{
				DoWorkerJob(_server_service);
			});
	}

	// main thread
	DoWorkerJob(_server_service);

	g_thread_manager->Join();
}