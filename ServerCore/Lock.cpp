#include "pch.h"

void Lock::WriteLock()
{
	// ���� �����尡 WL(WriteLock)�� ȹ���� ���� �������� �ʴ´�
	const uint32 thread_id = (_lock_flag & WRITE_THREAD_MASK) >> 16;
	if (thread_id == tls_thread_id)
	{
		++_write_count;
		return;
	}

	// time-out�� ����
	uint64 begin = GetTickCount64();
	// �ƹ��� �������� ���� �� WL�� ȹ���Ѵ�
	while (true)
	{
		for (int spin_count = 0; spin_count < MAX_SPIN_COUNT; ++spin_count)
		{
			// �ƹ��� ���� ȹ������ �ʾ��� �� WL�� ȹ��(w->r)
			uint32 expected = EMPTY_FLAG;
			uint32 desired = tls_thread_id << 16;
			if (_lock_flag.compare_exchange_weak(OUT expected, desired))
			{
				++_write_count;
				return;
			}
		}

		// �ʹ� ���� WL�� �� ��� ���� ���� ��Ȳ
		if (ACQUIRE_TIMEOUT_TICK <= GetTickCount64() - begin)
			CRASH("WriteLock time out!");

		this_thread::sleep_for(1ms);
	}
}

void Lock::WriteUnlock()
{
	// RL�� �� Ǯ��� WL�� �� Ǯ �� �ִ�
	// w->r�� ������ ���� �ɾ��ٸ� �ݵ�� r->w ������ ���� Ǯ�����
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
	// ���� �����尡 RL(ReadLock)�� ȹ���� ���� �������� �ʴ´�		
	const uint32 thread_id = (_lock_flag & WRITE_THREAD_MASK) >> 16;
	if (thread_id == tls_thread_id)
	{
		_lock_flag.fetch_add(1);
		return;
	}

	// WL�� ���� ���� �������� RL�� ���� �� �ִ�
	// WL->RL�� ��å�̹Ƿ� RL->WL�� ����� �� ����(����� ����)
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