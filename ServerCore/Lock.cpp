#include "pch.h"

void Lock::WriteLock()
{
	// 동일 스레드가 WL(WriteLock)을 획득할 때는 경합하지 않는다
	const uint32 thread_id = (_lock_flag & WRITE_THREAD_MASK) >> 16;
	if (thread_id == tls_thread_id)
	{
		++_write_count;
		return;
	}

	// time-out을 위해
	uint64 begin = GetTickCount64();
	// 아무도 소유하지 않을 때 WL을 획득한다
	while (true)
	{
		for (int spin_count = 0; spin_count < MAX_SPIN_COUNT; ++spin_count)
		{
			// 아무도 락을 획득하지 않았을 때 WL을 획득(w->r)
			uint32 expected = EMPTY_FLAG;
			uint32 desired = tls_thread_id << 16;
			if (_lock_flag.compare_exchange_weak(OUT expected, desired))
			{
				++_write_count;
				return;
			}
		}

		// 너무 오래 WL을 못 잡는 것은 문제 상황
		if (ACQUIRE_TIMEOUT_TICK <= GetTickCount64() - begin)
			CRASH("WriteLock time out!");

		this_thread::sleep_for(1ms);
	}
}

void Lock::WriteUnlock()
{
	// RL을 다 풀어야 WL을 다 풀 수 있다
	// w->r의 순서로 락을 걸었다면 반드시 r->w 순서로 락을 풀어야지
	if (0 != (_lock_flag.load() & READ_COUNT_MASK))
		CRASH("Invalid unlock error");

	const int32 lock_count = --_write_count;
	if (0 == lock_count)
	{
		_lock_flag.store(EMPTY_FLAG);
	}
}

void Lock::ReadLock()
{
	// 동일 스레드가 RL(ReadLock)을 획득할 때는 경합하지 않는다		
	const uint32 thread_id = (_lock_flag & WRITE_THREAD_MASK) >> 16;
	if (thread_id == tls_thread_id)
	{
		_lock_flag.fetch_add(1);
		return;
	}

	// WL을 잡지 않은 스레드라면 RL을 잡을 수 있다
	// WL->RL이 정책이므로 RL->WL은 허용할 수 없다(데드락 방지)
	uint64 begin = GetTickCount64();
	while (true)
	{
		for (int spin_count = 0; spin_count < MAX_SPIN_COUNT; ++spin_count)
		{
			uint32 expected = (_lock_flag & READ_COUNT_MASK);
			if (_lock_flag.compare_exchange_weak(expected, expected + 1))
				return;
		}

		if (ACQUIRE_TIMEOUT_TICK <= GetTickCount64() - begin)
			CRASH("ReadLock time out!");

		this_thread::sleep_for(1ms);
	}
}

void Lock::ReadUnlock()
{
	if (0 == (_lock_flag.fetch_sub(1) & READ_COUNT_MASK))
		CRASH("INVALID_UNLOCK");
}

// ReadLockGuard
ReadLockGuard::ReadLockGuard(Lock& lock) : _lock(lock)
{
#if defined(_DEBUG)
	BEFORE_LOCK(_lock);
#endif

	_lock.ReadLock();

#if defined(_DEBUG)
	AFTER_LOCK(_lock);
#endif
}

ReadLockGuard::~ReadLockGuard()
{
#if defined(_DEBUG)
	BEFORE_UNLOCK(_lock);
#endif

	_lock.ReadUnlock();
}

// WriteLockGuard
WriteLockGuard::WriteLockGuard(Lock& lock) : _lock(lock)
{
#if defined(_DEBUG)
	BEFORE_LOCK(_lock);
#endif

	_lock.WriteLock();

#if defined(_DEBUG)
	AFTER_LOCK(_lock);
#endif
}

WriteLockGuard::~WriteLockGuard()
{
#if defined(_DEBUG)
	BEFORE_UNLOCK(_lock);
#endif

	_lock.WriteUnlock();
}