#include "pch.h"

DeadLockDetector& DeadLockDetector::Instance()
{
	static DeadLockDetector instance;
	return instance;
}

void DeadLockDetector::BeforeLock(Lock& lock, string func_name, int32 line)
{
	LockGuard guard(_lock);

	thread::id current_thread = this_thread::get_id();
	_wait_map[current_thread].push_back(&lock);

	auto find_iter = _owner_map.find(&lock);
	// 현재 락의 주인 스레드가 없거나 스스로를 재귀적으로 획득하는 거면 사이클이 없다
	if (find_iter == _owner_map.end() || find_iter->second == current_thread)
		return;

	unordered_set<thread::id> visited_thread;
	if (CheckCycle(current_thread, find_iter->second, visited_thread))
	{
		std::cerr << "Deadlock detected between thread " << current_thread
			<< " and " << find_iter->second;
		if (!func_name.empty()) std::cerr << " at " << func_name << ":" << line;
		std::cerr << std::endl;
		throw std::runtime_error("Deadlock detected");
	}
}

void DeadLockDetector::AfterLock(Lock& lock)
{
	thread::id current_thread = this_thread::get_id();
	LockGuard guard(_lock);	

	auto& vector_lock = _wait_map.at(current_thread);

	auto find_lock = std::find(vector_lock.begin(), vector_lock.end(), &lock);
	if (find_lock != vector_lock.end())
		vector_lock.erase(find_lock);

	auto find_owner = _owner_map.find(&lock);
	if (find_owner != _owner_map.end() && find_owner->second == current_thread)
	{
		_owner_count[current_thread]++;
	}
	else
	{
		_owner_count[current_thread] = 1;
		_owner_map[&lock] = current_thread;
	}
}

void DeadLockDetector::BeforeUnlock(Lock& lock)
{
	thread::id current_thread = this_thread::get_id();
	LockGuard guard(_lock);

	auto find_iter = _owner_map.find(&lock);

	if (find_iter == _owner_map.end() || find_iter->second != current_thread)
		throw std::runtime_error("Invalid unlock");

	int32 count = --_owner_count[current_thread];
	if (count == 0)
		_owner_map.erase(find_iter);
	else if (count < 0)
		throw std::runtime_error("Invalid unlock");
}

bool DeadLockDetector::CheckCycle(thread::id start_thread, thread::id current_thread, unordered_set<thread::id>& visited_thread)
{
	if (!visited_thread.insert(current_thread).second)
		return false;

	auto find_iter = _wait_map.find(current_thread);
	if (find_iter == _wait_map.end())
		return false;

	for (Lock* lk : find_iter->second)
	{
		auto find_owner = _owner_map.find(lk);
		if (find_owner == _owner_map.end())
			continue;

		thread::id next_thread_id = find_owner->second;
		// cycle
		if (next_thread_id == start_thread)
			return true;

		if (CheckCycle(start_thread, next_thread_id, visited_thread))
			return true;
	}

	return false;
}
