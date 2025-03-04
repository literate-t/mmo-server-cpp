#pragma once

struct JobData
{
	JobData(weak_ptr<JobQueue> owner_queue, SharedJob job) : owner_queue(owner_queue), job(job) {}

	weak_ptr<JobQueue> owner_queue;
	SharedJob job;
};

struct TimerItem
{
	bool operator<(const TimerItem& other) const
	{
		return executeTime > other.executeTime;
	}

	uint64 executeTime = 0;
	// 자료구조 안에서 수시로 구조가 변하면서 복사될 것이므로
	// 복사 비용을 최소화하기 위해
	JobData* job_data = nullptr;
};

class JobTimer
{
public:
	void ReserveJob(uint64 delay_time, weak_ptr<JobQueue> owner_queue, SharedJob job);
	void DistributeJobs(uint64 now);
	void Clear();

private:
	USE_LOCK;
	xpriority_queue<TimerItem> _timer_items;
	Atomic<bool> _processing = false;
};
