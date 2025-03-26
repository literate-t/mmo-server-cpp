#include "pch.h"
#include "Player.h"
#include "ViewCube.h"

Player::Player()	
{
	_object_type = GameObjectType::PLAYER;
	_view_cube = MakeShared<ViewCube>(static_pointer_cast<Player>(shared_from_this()));
}

void Player::RefreshStat()
{
}