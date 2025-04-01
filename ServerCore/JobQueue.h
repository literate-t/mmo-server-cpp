#pragma once

#include "LockQueue.h"
#include "Job.h"
#include "JobTimer.h"

// Job 객체에 일감을 저장할 때, 일감 대상인 클래스는 JobQueue를 상속한다
class JobQueue : public enable_shared_from_this<JobQueue>
{
public:
	// for lamda
	void PushJobAsync(CallbackType&& callback)
	{
		Push(ObjectPool<Job>::MakeShared(move(callback)));		
	}

	template<typename T, typename Return, typename...Args1, typename...Args2>
	void PushJobAsync(Return(T::*member_func)(Args1...), Args2&&...args)
	{
		// 일감 요청이 발생하는 곳(ex.Room : public JobQueue)
		// room->PushJob(&Room::Enter, move(player));
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<Job>::MakeShared(owner, member_func, forward<Args2>(args)...));		
	}

	// for lamda
	SharedJob PushTimerAsync(uint64 delay_time, CallbackType&& callback)
	{
		SharedJob job = ObjectPool<Job>::MakeShared(move(callback));
		g_job_timer->ReserveJob(delay_time, shared_from_this(), job);

		return job;
	}

	template<typename T, typename Return, typename...Args1, typename...Args2>
	SharedJob PushTimerAsync(uint64 delay_time, Return(T::* member_func)(Args1...), Args2&&...args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		SharedJob job = ObjectPool<Job>::MakeShared(owner, member_func, forward<Args2>(args)...);
		g_job_timer->ReserveJob(delay_time, owner, job);

		return job;
	}

	void Execute();
	void Push(const SharedJob& job, bool push_only = false);

protected:
	LockQueue<SharedJob> _jobs;
	Atomic<int32> _job_count = 0;
};
