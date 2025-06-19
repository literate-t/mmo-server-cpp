#pragma once

#define OUT

// MemoryPool
#define _SIZE_POOL

//#define _TLS_SLAB

// Stomp
//#define _STOMP

// ------- Lock ------- //
#define USE_MANY_LOCKS(count)	Lock _locks[count];
#define USE_LOCK				USE_MANY_LOCKS(1)
// typeid(this).name() -> XxxLockGuard를 사용하는 클래스의 객체의 타입을 구한다
#define READ_LOCK_IDX(index)	ReadLockGuard read_lock_guard_##index(_locks[index]);
#define READ_LOCK				READ_LOCK_IDX(0)
#define WRITE_LOCK_IDX(index)	WriteLockGuard write_lock_guard_##index(_locks[index]);
#define WRITE_LOCK				WRITE_LOCK_IDX(0)

#define BEFORE_LOCK(lock)		DeadLockDetector::Instance().BeforeLock(lock, __FUNCTION__, __LINE__)
#define AFTER_LOCK(lock)		DeadLockDetector::Instance().AfterLock(lock)
#define BEFORE_UNLOCK(lock)		DeadLockDetector::Instance().BeforeUnlock(lock)

// ------- CRASH ------- //
// 강제로 crash 낼 때 사용한다
// __analysis_assume(expr)은 expr를 true로 간주
#define CRASH(cause)						\
{											\
	uint32* crash = nullptr;				\
	__analysis_assume(crash != nullptr);	\
	*crash = 0xDEADBEEF;					\
}

// expr이 false이면 crash 발생
#define ASSERT_CRASH(expr)					\
{											\
	if (!(expr))							\
	{										\
		CRASH("ASSERT_CRASH");				\
	}										\
}