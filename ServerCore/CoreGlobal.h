#pragma once

extern class SendBufferManager*		g_send_buffer_manager;
extern class GlobalQueue*			g_global_queue;
extern class JobTimer*				g_job_timer;
extern class DBConnectionPool*		g_db_connection_pool;

// 전역으로 사용될 매니저 역할을 할 것을 모아둔다
// 객체를 생성하는 순서가 필요할 수도 있다
class CoreGlobal
{
public:
	CoreGlobal();
	~CoreGlobal();
};