#pragma once

#include <thread>
#include <functional>

/*------------------
	ThreadManager
------------------*/
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
	void Join();

	static void InitTLS();
	static void DestroyTLS();

	static void WorkGlobalQueue();
	static void DistributeReservedJobs();

private:
	Mutex			_lock;
	vector<thread>	_threads;
};