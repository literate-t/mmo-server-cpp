#pragma once

#include <thread>
#include <functional>

class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	inline static ThreadManager& Instance()
	{
		static ThreadManager instance;
		return instance;
	}

	void Start(function<void(void)> callback);
	void Join();

	static void InitTLS();
	static void DestroyTLS();

	static void WorkGlobalQueue();
	static void DistributeReservedJobs();

private:
	Mutex			_lock;
	xvector<thread>	_threads;
};