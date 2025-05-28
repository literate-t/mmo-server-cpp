#pragma once

/*------------------------------------------
	[wwwwwwww][wwwwwwww][rrrrrrrr][rrrrrrrr]
	w: write flag(Exclusive lock owner flag)
	r: read flag(Shared lock count)
------------------------------------------*/
/* ��å(���� ������ ����) */
// w -> r(o)
// w -> w(o)
// ���� ���� ���� ������� ��ø�ؼ� ����, �б� ���� ���� ���� �� ����
// r -> w(x)
// �б� ���� �ɷ� ������ ���� ���� ���� �� ����

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
	// ���� ���� �����常 ������ �Ŷ� atomic���� ������ �ʾƵ� �ȴ�
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