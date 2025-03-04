#include "pch.h"
#include "JobQueue.h"
#include "GlobalQueue.h"

void JobQueue::Push(const SharedJob& job, bool push_only)
{
	// count를 올리고 job을 넣는 순서가 중요하다
	const int32 prev_count = _job_count.fetch_add(1);
	_jobs.Push(job);

	// 첫 번째 job을 넣은 스레드가 실행을 담당
	if (0 == prev_count)
	{
		if (push_only == false && nullptr == tls_current_job_queue)
		{
			Execute();
		}
		else
		{			
			// 다른 스레드가 실행할 수 있도록
			g_global_queue->Push(shared_from_this());
		}
	}
}

void JobQueue::Execute()
{
	tls_current_job_queue = this;
	while (true)
	{
		xvector<SharedJob> jobs;
		_jobs.PopAll(OUT jobs);

		const int32 job_size = static_cast<int32>(jobs.size());
		for (int i = 0; i < job_size; ++i)
		{
			jobs[i]->Execute();
		}

		// _job_count가 0이 될 때까지 반복된다
		if (job_size == _job_count.fetch_sub(job_size))
		{
			tls_current_job_queue = nullptr;
			return;
		}

		// 너무 많은 일감이 있을 때 그만하고 다른 스레드에 넘긴다
		const uint64 now = GetTickCount64();
		if (tls_end_tick_count <= now)
		{
			tls_current_job_queue = nullptr;
			g_global_queue->Push(shared_from_this());
			return;
		}
	}
}
