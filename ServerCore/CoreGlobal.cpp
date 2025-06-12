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

/* CoreGlobal ��ü ���� */
// ���� ������
CoreGlobal core_global;

// �����Ϸ� ����ȭ�� ���� ���� ������ core_global ��ü�� ������ ������ ������
// �ƿ� ������ �� �� �� ����
void ForceUse()
{
	core_global;
}