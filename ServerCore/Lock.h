#pragma once

/*------------------------------------------
	[wwwwwwww][wwwwwwww][rrrrrrrr][rrrrrrrr]
	w: write flag(Exclusive lock owner flag)
	r: read flag(Shared lock count)
------------------------------------------*/
/* 정책(동일 스레드 기준) */
// w -> r(o)
// w -> w(o)
// 쓰기 락을 잡은 스레드는 중첩해서 쓰기, 읽기 락을 전부 잡을 수 있음
// r -> w(x)
// 읽기 락이 걸려 있으면 쓰기 락은 잡을 수 없음

// --------- Lock --------- //
class Lock
{
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK	= 10000,
		MAX_SPIN_COUNT			= 5000,
		WRITE_THREAD_MASK		= 0xFFFF'0000,
		READ_COUNT_MASK			= 0x0000'FFFF,
		EMPTY_FLAG				= 0x0000'0000
	};

public:
	void WriteLock();
	void WriteUnlock();
	void ReadLock();
	void ReadUnlock();

private:
	Atomic<uint32> _lock_flag;
	// 락을 잡은 스레드만 접근할 거라 atomic으로 만들지 않아도 된다
	uint16 _write_count = 0;
};

// -------- ReadLockGuard -------- //
class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock);
	~ReadLockGuard();

private:
	Lock& _lock;
};

// -------- WriteLockGuard -------- //
class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock);
	~WriteLockGuard();

private:
	Lock& _lock;
};