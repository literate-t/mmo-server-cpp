#pragma once

extern class ThreadManager*			g_thread_manager;
extern class MemoryManager*			g_memory_manager;
extern class SendBufferManager*		g_send_buffer_manager;
extern class GlobalQueue*			g_global_queue;
extern class JobTimer*				g_job_timer;
extern class DBConnectionPool*		g_db_connection_pool;

// �������� ���� �Ŵ��� ������ �� ���� ��Ƶд�
// ��ü�� �����ϴ� ������ �ʿ��� ���� �ִ�
class CoreGlobal
{
public:
	CoreGlobal();
	~CoreGlobal();
};