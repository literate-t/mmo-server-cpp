#pragma once

class Player
{
public:
	uint64 PlayerDbId = 0;
	string name;
	//Protocol::PlayerType type = Protocol::PLAYER_TYPE_NONE;
	// caution: cycle
	SharedGameSession OwnerSession;

};