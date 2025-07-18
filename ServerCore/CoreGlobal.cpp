#include "pch.h"

GlobalQueue*		g_global_queue			= nullptr;
JobTimer*			g_job_timer				= nullptr;
DBConnectionPool*	g_db_connection_pool	= nullptr;

CoreGlobal::CoreGlobal()
{
	g_global_queue			= new GlobalQueue();
	g_job_timer				= new JobTimer();
	g_db_connection_pool	= new DBConnectionPool();
	SocketUtils::Init();
}

CoreGlobal::~CoreGlobal()
{
	delete g_global_queue;
	delete g_job_timer;
	delete g_db_connection_pool;
	SocketUtils::Clear();
}

/* CoreGlobal 객체 생성 */
// 메인 스레드
CoreGlobal core_global;

// 컴파일러 최적화로 인해 전역 변수나 core_global 객체가 실제로 사용되지 않으면
// 아예 생성이 안 될 수 있음
void ForceUse()
{
	core_global;
}