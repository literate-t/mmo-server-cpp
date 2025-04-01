#include "pch.h"
#include "Job.h"

void Job::Cancel()
{
	_callback = []() {};
}