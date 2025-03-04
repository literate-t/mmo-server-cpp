#include "pch.h"
#include "PlayerManager.h"
#include "SessionManager.h"

PlayerManager g_player_manager;

void PlayerManager::PlayerThenSession()
{
	WRITE_LOCK;
	g_session_manager.Lock();
}

void PlayerManager::Lock()
{
	WRITE_LOCK;	
}