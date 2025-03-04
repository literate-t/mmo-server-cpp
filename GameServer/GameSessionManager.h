#pragma once

class GameSessionManager
{
public:
	void Add(SharedGameSession session);
	void Remove(SharedGameSession session);
	void Broadcast(SharedSendBuffer send_buffer);

private:
	USE_LOCK;

	xset<SharedGameSession> _sessions;
};

extern GameSessionManager g_session_manager;