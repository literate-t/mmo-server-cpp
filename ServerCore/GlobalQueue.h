#pragma once

class GlobalQueue
{
public:
	GlobalQueue();
	~GlobalQueue();

	void Push(SharedJobQueue job_queue);
	SharedJobQueue Pop();

private:
	LockQueue<SharedJobQueue> _job_queues;
};