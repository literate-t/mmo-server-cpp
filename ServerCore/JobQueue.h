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

	template<typename T, typename Return, typename...Args>
	void PushJobAsync(Return(T::*member_func)(Args...), Args&&...args)
	{
		// 일감 요청이 발생하는 곳(ex.Room : public JobQueue)
		// room->PushJob(&Room::Enter, move(player));
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<Job>::MakeShared(owner, member_func, forward<Args>(args)...));		
	}

	// for lamda
	void PushTimerAsync(uint64 delay_time, CallbackType&& callback)
	{
		SharedJob job = ObjectPool<Job>::MakeShared(move(callback));
		g_job_timer->ReserveJob(delay_time, shared_from_this(), job);
	}

	template<typename T, typename Return, typename...Args>
	void PushTimerAsync(uint64 delay_time, Return(T::* member_func)(Args...), Args&&...args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		SharedJob job = ObjectPool<Job>::MakeShared(owner, member_func, forward<Args>(args)...);
		g_job_timer->ReserveJob(delay_time, owner, job);
	}

	void Execute();
	void Push(const SharedJob& job, bool push_only = false);

protected:
	LockQueue<SharedJob> _jobs;
	Atomic<int32> _job_count = 0;
};
