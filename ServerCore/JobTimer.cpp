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
	// 한 번에 하나의 스레드만 수행한다
	if (true == _processing.exchange(true))
		return;

	xvector<TimerItem> items;
	{
		WRITE_LOCK;
		items.reserve(_timer_items.size());
		while (false == _timer_items.empty())
		{
			const TimerItem& item = _timer_items.top();
			// 현재의 틱이 가장 가까운 실행 시점에 도달하지 않았다면 바로 종료
			if (now < item.executeTime)
				break;

			items.push_back(item);
			_timer_items.pop();
		}
	}

	for (const TimerItem& item : items)
	{
		// 이미 메모리가 해제된 JobQueue라면 일감을 추가하지 않는다
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

