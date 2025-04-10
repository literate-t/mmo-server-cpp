#include "pch.h"
#include "Monster.h"
#include "DataManager.h"
#include "Room.h"
#include "Map.h"
#include "Player.h"
#include "ClientPacketHandler.h"

Monster::Monster()
	:GameObject(GameObjectType::MONSTER)
{
}

Monster::~Monster()
{
	cout << "~Monster()" << endl;
}
