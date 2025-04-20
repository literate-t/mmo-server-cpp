#pragma once

class GameSessionManager
{
public:
	void Add(SharedGameSession session);
	void Remove(SharedGameSession session);
	void Broadcast(SharedSendBuffer send_buffer);
	void FlushSend();

private:
	USE_LOCK;

	xset<SharedGameSession> _sessions;
};
