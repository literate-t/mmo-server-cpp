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
	// �����尡 �� ȹ�� ��û ���(T -> R)
	unordered_map<thread::id, vector<Lock*>> _wait_map;
	// �����尡 �����ϴ� ��(R -> T)
	unordered_map<Lock*, thread::id> _owner_map;	
};
