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
	xvector<SharedSession> session_snapshot;
	{
		READ_LOCK;
		session_snapshot.reserve(_sessions.size());
		session_snapshot.insert(session_snapshot.end(), _sessions.begin(), _sessions.end());
	}

	for (auto& session : session_snapshot)
		session->FlushSend();
}
