#pragma once
class Heartbeat : public JobQueue
{
public:
	void Ping(SharedGameSession owner);
	void HandlePong(SharedGameSession owner);
};

