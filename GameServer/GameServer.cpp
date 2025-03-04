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
	//// DB Test
	ASSERT_CRASH(g_db_connection_pool->Connect(1, L"Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\MSSQLLocalDB;Database=ServerDB;Trusted_Connection=Yes;"));

	// Creat table
	{
		auto query = L"									\
			DROP TABLE IF EXISTS [dbo].[Gold];			\
			CREATE TABLE [dbo].[Gold]					\
			(											\
				[id] INT NOT NULL PRIMARY KEY IDENTITY, \
				[gold] INT NULL,						\
				[name] NVARCHAR(50) NULL,				\
				[createDate] DATETIME NULL				\
			);";

		DBConnection* dbConn = g_db_connection_pool->Pop();
		ASSERT_CRASH(dbConn->Execute(query));
		g_db_connection_pool->Push(dbConn);
	}

	// Add data
	for (int32 i = 0; i < 3; ++i)
	{
		DBConnection* dbConn = g_db_connection_pool->Pop();

		DBBind<3, 0> db_bind(*dbConn, L"INSERT INTO [dbo].[Gold]([gold], [name], [createDate]) VALUES(?, ?, ?);");

		int32 gold = 100;
		WCHAR name[100] = L"DB테스트";
		TIMESTAMP_STRUCT ts{ 2025, 2, 28 };

		db_bind.BindParam(0, gold);
		db_bind.BindParam(1, name);
		db_bind.BindParam(2, ts);

		db_bind.Execute();
		//// 넘길 인자 바인딩
		//dbConn->Unbind();
		//int32 gold = 100;
		//SQLLEN len = 0; // value가 정수일 때는 0을 넘기면 된다
		//WCHAR name[100] = L"팍씨";
		//SQLLEN name_length = SQL_NTSL;

		//TIMESTAMP_STRUCT ts{};
		//ts.year = 2025;
		//ts.month = 2;
		//ts.day = 28;
		//SQLLEN ts_len = 0;
		//ASSERT_CRASH(dbConn->BindParam(1, &gold, &len));
		//ASSERT_CRASH(dbConn->BindParam(2, name, &name_length));
		//ASSERT_CRASH(dbConn->BindParam(3, &ts, &ts_len));

		//// SQL 실행
		//ASSERT_CRASH(dbConn->Execute(L"INSERT INTO [dbo].[Gold]([gold], [name], [createDate]) VALUES(?, ?, ?);"));
		g_db_connection_pool->Push(dbConn);
	}

	// Read
	{
		DBConnection* dbConn = g_db_connection_pool->Pop();

		DBBind<1, 4> db_bind(*dbConn, L"SELECT id, gold, name, createDate FROM [dbo].[Gold] WHERE gold = (?);");
		int32 gold = 100;
		int32 out_id = 0;
		int32 out_gold = 0;
		WCHAR out_name[100];
		TIMESTAMP_STRUCT out_date{};

		db_bind.BindParam(0, gold);
		db_bind.BindColumn(0, out_id);
		db_bind.BindColumn(1, out_gold);
		db_bind.BindColumn(2, out_name);
		db_bind.BindColumn(3, out_date);
		db_bind.Execute();

		//dbConn->Unbind();

		//// 넘길 인자 바인딩
		//int32 gold = 100;
		//SQLLEN len = 0; // value가 정수일 때는 0을 넘기면 된다
		//ASSERT_CRASH(dbConn->BindParam(1, &gold, &len));

		//int32 out_id = 0;
		//SQLLEN out_id_len = 0;
		//ASSERT_CRASH(dbConn->BindColumn(1, &out_id, &out_id_len));

		//int32 out_gold = 0;
		//SQLLEN out_gold_len = 0;
		//ASSERT_CRASH(dbConn->BindColumn(2, &out_gold, &out_gold_len));

		//WCHAR out_name[100];
		//SQLLEN out_name_len = 0;
		//ASSERT_CRASH(dbConn->BindColumn(3, out_name,length32(out_name), &out_name_len));

		//TIMESTAMP_STRUCT out_date{};
		//SQLLEN out_date_len = 0;
		//ASSERT_CRASH(dbConn->BindColumn(4, &out_date, &out_date_len));

		//ASSERT_CRASH(dbConn->Execute(L"SELECT id, gold, name, createDate FROM [dbo].[Gold] WHERE gold = (?);"));

		wcout.imbue(locale("kor"));
		//while (dbConn->Fetch())
		while (db_bind.Fetch())
		{
			wcout << "id:" << out_id << ", gold: " << out_gold << ", name:" << out_name
				<< ", createData: " << out_date.year << "/" << out_date.month << "/" << out_date.day << endl;
		}

		g_db_connection_pool->Push(dbConn);
	}

	ClientPacketHandler::Init();
	SharedServerService _server_service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 9000),
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