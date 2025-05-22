#pragma once

class SessionManager
{
public:
	SessionManager() = default;
	SessionManager(const SessionManager& rhs) = delete;
	SessionManager& operator=(const SessionManager&) = delete;

	void Add(SharedSession session);
	void Remove(SharedSession session);
	void Broadcast(SharedSendBuffer send_buffer);
	void FlushSend();

private:
	USE_LOCK;

	xset<SharedSession> _sessions;
};