#include "pch.h"
#include "GameSessionManager.h"
#include "GameSession.h"

GameSessionManager g_session_manager;

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
