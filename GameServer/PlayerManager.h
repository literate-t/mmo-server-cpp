#pragma once
class PlayerManager
{
	USE_LOCK;

public:
	void PlayerThenSession();
	void Lock();
};

extern PlayerManager g_player_manager;