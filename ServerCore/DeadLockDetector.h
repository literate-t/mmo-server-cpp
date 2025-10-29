#pragma once

class DeadLockDetector
{
public:
	inline static DeadLockDetector& Instance()
	{
		static DeadLockDetector instance;
		return instance;
	}

	void BeforeLock(Lock& lock, string func_name = "", int32 line = 0);
	void AfterLock(Lock& lock);
	void BeforeUnlock(Lock& lock);

private:
	bool CheckCycle(thread::id current_thread, thread::id next_thread, unordered_set<thread::id>& visited_thread);

private:
	Mutex _lock;
	unordered_map<thread::id, int32> _owner_count;
	// �����尡 �� ȹ�� ��û ���(T -> R)
	unordered_map<thread::id, vector<Lock*>> _wait_map;
	// �����尡 �����ϴ� ��(R -> T)
	unordered_map<Lock*, thread::id> _owner_map;	
};
