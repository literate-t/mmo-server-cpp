#include "pch.h"
#include "JobTimer.h"
#include "JobQueue.h"

void JobTimer::ReserveJob(uint64 delay_time, weak_ptr<JobQueue> owner_queue, SharedJob job)
{
	uint64 execute_time = GetTickCount64() + delay_time;
	JobData* job_data = ObjectPool<JobData>::Pop(owner_queue, job);

	WRITE_LOCK;
	_timer_items.push(TimerItem{ execute_time , job_data});
}

void JobTimer::DistributeJobs(uint64 now)
{
	// �� ���� �ϳ��� �����常 �����Ѵ�
	if (true == _processing.exchange(true))
		return;

	xvector<TimerItem> items;
	{
		WRITE_LOCK;
		items.reserve(_timer_items.size());
		while (false == _timer_items.empty())
		{
			const TimerItem& item = _timer_items.top();
			// ������ ƽ�� ���� ����� ���� ������ �������� �ʾҴٸ� �ٷ� ����
			if (now < item.executeTime)
				break;

			items.push_back(item);
			_timer_items.pop();
		}
	}

	for (const TimerItem& item : items)
	{
		// �̹� �޸𸮰� ������ JobQueue��� �ϰ��� �߰����� �ʴ´�
		if (const SharedJobQueue queue = item.job_data->owner_queue.lock())
			queue->Push(item.job_data->job);

		ObjectPool<JobData>::Push(item.job_data);
	}

	_processing.store(false);
}

void JobTimer::Clear()
{
	WRITE_LOCK;
	while (false == _timer_items.empty())
	{
		const TimerItem& item = _timer_items.top();
		ObjectPool<JobData>::Push(item.job_data);
		_timer_items.pop();
	}
}

