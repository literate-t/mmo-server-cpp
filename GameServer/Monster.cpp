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

void Monster::Init(int32 data_sheet_id)
{
	_data_sheet_id = data_sheet_id;
	MonsterData& data = g_data_manager->Monster(_data_sheet_id);
	SetStatInfoWithStatData(data.stat);
	SetState(EntityState::IDLE);
}

// FSM
void Monster::Update()
{
	_room = GetRoom();
	if (nullptr == _room)
		return;

	switch (GetState())
	{
	case EntityState::IDLE:
		UpdateIdle();
		break;
	case EntityState::MOVING:
		UpdateMoving();
		break;
	case EntityState::SKILL:
		UpdateSkill();
		break;
	case EntityState::DEAD:
		UpdateDead();
		break;
	}

	auto shared_this = shared_from_this();
	JobReserved = _room->PushTimerAsync(200, [shared_this]() { shared_this->Update(); });
}