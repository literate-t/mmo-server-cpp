#include "pch.h"
#include "GameSessionManager.h"
#include "GameSession.h"

void GameSessionManager::Add(SharedGameSession session)
{
	WRITE_LOCK;
	_sessions.insert(session);
}

void GameSessionManager::Remove(SharedGameSession session)
{
	WRITE_LOCK;
	_sessions.erase(session);
}

void GameSessionManager::Broadcast(SharedSendBuffer send_buffer)
{
	WRITE_LOCK;
	for (SharedGameSession session : _sessions)
	{
		session->Send(send_buffer);
	}
}

void GameSessionManager::FlushSend()
{
	xset<SharedGameSession> sessions;
	{
		READ_LOCK;
		sessions = _sessions;
	}

	for (auto& session : sessions)
		session->FlushSend();
}
