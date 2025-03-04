#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ClientPacketHandler.h"
#include "Room.h"

GameSession::~GameSession()
{
	puts("~GameSession");
}

void GameSession::OnConnectCompleted()
{
	g_session_manager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnectCompleted()
{
	g_session_manager.Remove(static_pointer_cast<GameSession>(shared_from_this()));

	// player와의 순환을 끊어준다
	if (_current_player)
	{
		if (auto room = _room.lock())
		{
			room->PushJobAsync(&Room::Leave, move(_current_player));
		}
	}

	_current_player = nullptr;
	_players.clear();
}

void GameSession::OnRecvPacketProcessed(BYTE* buffer, int32 length)
{
	SharedPacketSession session = GetSharedPacketSession();
	ClientPacketHandler::HandlePacket(session, buffer, length);
}

void GameSession::OnSendCompleted(int32 length)
{
	
}

void GameSession::AddPlayer(SharedPlayer player)
{
	_players.push_back(player);
}

void GameSession::RemovePlayer(SharedPlayer player)
{
	auto find_iter = find(_players.begin(), _players.end(), player);
	_players.erase(find_iter);
}
