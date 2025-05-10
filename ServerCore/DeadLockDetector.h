#pragma once

/*---------------------
	DeadLockDetector
---------------------*/
class DeadLockDetector
{
public:
	static DeadLockDetector& Instance();
	void BeforeLock(Lock& lock, string func_name = "", int32 line = 0);
	void AfterLock(Lock& lock);
	void BeforeUnlock(Lock& lock);

private:
	bool CheckCycle(thread::id start_thread, thread::id current_thread, unordered_set<thread::id>& visited_thread);

private:
	Mutex _lock;
	unordered_map<thread::id, int32> _owner_count;
	// 스레드가 락 획득 요청 대기(T -> R)
	unordered_map<thread::id, vector<Lock*>> _wait_map;
	// 스레드가 소유하는 락(R -> T)
	unordered_map<Lock*, thread::id> _owner_map;	
};
