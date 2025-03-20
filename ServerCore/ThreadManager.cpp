#include "pch.h"

/*------------------
	ThreadManager
------------------*/
ThreadManager::ThreadManager()
{
	// Main thread
	InitTLS();
}

ThreadManager::~ThreadManager()
{
}

void ThreadManager::Launch(function<void(void)> callback)
{
	LockGuard guard(_lock);

	// callback�� �� ����
	// �� �������� ���� �ڿ��� �����Ϸ��� �� ���縦 �ؾ� �Ѵ�
	_threads.push_back(thread([callback]()
		{
			try
			{
				InitTLS();
				callback();
			}
			catch (exception& e)
			{
				printf("Exception in thread[%d]: %s\n", tls_thread_id, e.what());
			}

			DestroyTLS();
		}));
}

void ThreadManager::Join()
{
	for (thread& t : _threads)
	{
		if (t.joinable())
			t.join();
	}

	_threads.clear();
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> id_thread = 1;
	tls_thread_id = id_thread.fetch_add(1);
}

void ThreadManager::DestroyTLS()
{
}

void ThreadManager::WorkGlobalQueue()
{
	while (true)
	{
		// TODO : �ǹ� ���� �ڵ��� ���������� �ƿ� �����ϴ� �� ����
		//uint64 now = GetTickCount64();
		//if (tls_end_tick_count < now)
		//{
		//	break;
		//}

		SharedJobQueue job_queue = g_global_queue->Pop();
		if (nullptr == job_queue)
		{
			break;
		}

		job_queue->Execute();
	}
}

void ThreadManager::DistributeReservedJobs()
{
	const int64 now = GetTickCount64();
	g_job_timer->DistributeJobs(now);
}
