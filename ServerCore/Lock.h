#pragma once

/*---------------------------
	Reader/Writer SpinLock
---------------------------*/
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
	void WriteLock(const char* name);
	void WriteUnlock(const char* name);
	void ReadLock(const char* name);
	void ReadUnlock(const char* name);

private:
	Atomic<uint32> _lock_flag;
	// ���� ���� �����常 ������ �Ŷ� atomic���� ������ �ʾƵ� �ȴ�
	uint16 _write_count = 0;
};

/*---------------
	LockGurad
----------------*/
class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name)
	{
		_lock.ReadLock(name);
	}

	~ReadLockGuard()
	{
		_lock.ReadUnlock(_name);
	}

private:
	Lock& _lock;
	const char* _name;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock, const char* name) : _lock(lock), _name(name)
	{
		_lock.WriteLock(name);
	}

	~WriteLockGuard()
	{
		_lock.WriteUnlock(_name);
	}

private:
	Lock& _lock;
	const char* _name;
};