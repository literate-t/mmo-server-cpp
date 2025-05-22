#include "pch.h"
#include "SessionManager.h"

void SessionManager::Add(SharedSession session)
{
	WRITE_LOCK;
	_sessions.insert(session);
}

void SessionManager::Remove(SharedSession session)
{
	WRITE_LOCK;
	_sessions.erase(session);
}

void SessionManager::Broadcast(SharedSendBuffer send_buffer)
{
	WRITE_LOCK;
	for (SharedSession session : _sessions)
	{
		session->Send(send_buffer);
	}
}

void SessionManager::FlushSend()
{
	std::vector<SharedSession> session_snapshot;
	{
		READ_LOCK;
		session_snapshot.reserve(_sessions.size());
		for (auto& s : _sessions)
			session_snapshot.push_back(s);
	}

	for (auto& session : session_snapshot)
		session->FlushSend();
}
