#include "pch.h"

ThreadManager*		g_thread_manager		= nullptr;
DeadLockDetector*	g_deadlock_detector		= nullptr;
MemoryManager*		g_memory_manager		= nullptr;
SendBufferManager*	g_send_buffer_manager	= nullptr;
GlobalQueue*		g_global_queue			= nullptr;
JobTimer*			g_job_timer				= nullptr;
DBConnectionPool*	g_db_connection_pool	= nullptr;

CoreGlobal::CoreGlobal()
{
	g_thread_manager		= new ThreadManager();
	g_deadlock_detector		= new DeadLockDetector();
	g_memory_manager		= new MemoryManager();
	g_send_buffer_manager	= new SendBufferManager();
	g_global_queue			= new GlobalQueue();
	g_job_timer				= new JobTimer();
	g_db_connection_pool	= new DBConnectionPool();
	SocketUtils::Init();
}

CoreGlobal::~CoreGlobal()
{
	delete g_thread_manager;
	delete g_deadlock_detector;
	delete g_memory_manager;
	delete g_send_buffer_manager;
	delete g_global_queue;
	delete g_job_timer;
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