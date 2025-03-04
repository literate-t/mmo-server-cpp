#include "pch.h"
#include "JobQueue.h"
#include "GlobalQueue.h"

void JobQueue::Push(const SharedJob& job, bool push_only)
{
	// count�� �ø��� job�� �ִ� ������ �߿��ϴ�
	const int32 prev_count = _job_count.fetch_add(1);
	_jobs.Push(job);

	// ù ��° job�� ���� �����尡 ������ ���
	if (0 == prev_count)
	{
		if (push_only == false && nullptr == tls_current_job_queue)
		{
			Execute();
		}
		else
		{			
			// �ٸ� �����尡 ������ �� �ֵ���
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

		// _job_count�� 0�� �� ������ �ݺ��ȴ�
		if (job_size == _job_count.fetch_sub(job_size))
		{
			tls_current_job_queue = nullptr;
			return;
		}

		// �ʹ� ���� �ϰ��� ���� �� �׸��ϰ� �ٸ� �����忡 �ѱ��
		const uint64 now = GetTickCount64();
		if (tls_end_tick_count <= now)
		{
			tls_current_job_queue = nullptr;
			g_global_queue->Push(shared_from_this());
			return;
		}
	}
}
