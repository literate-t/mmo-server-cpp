#include "pch.h"
#include "GlobalQueue.h"

GlobalQueue::GlobalQueue()
{
}

GlobalQueue::~GlobalQueue()
{
}

void GlobalQueue::Push(SharedJobQueue job_queue)
{
	_job_queues.Push(job_queue);
}

SharedJobQueue GlobalQueue::Pop()
{
	return _job_queues.Pop();
}
