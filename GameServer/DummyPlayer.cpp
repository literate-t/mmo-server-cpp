#include "pch.h"
#include "DummyPlayer.h"
#include "Room.h"
#include "Monster.h"
#include "DataManager.h"
#include "Map.h"
#include "Player.h"
#include "ClientPacketHandler.h"
#include "Random.h"
#include "DBSerializer.h"

DummyPlayer::DummyPlayer()
	:Player(true),
	_stat_data(nullptr),
	_skill_data(nullptr)
{
}

DummyPlayer::~DummyPlayer()
{
	puts("~DummyPlayer()");
}

void DummyPlayer::Update()
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
	_job_reserved = _room->PushTimerAsync(200, [shared_this]() { shared_this->Update(); });
}

void DummyPlayer::OnDead(SharedObject attacker)
{
	_job_reserved->Cancel();

	GameObject::OnDead(attacker);
}

void DummyPlayer::UpdateIdle()
{
	if (_search_tick > GetTickCount64())
		return;
	_search_tick = GetTickCount64() + 1000;

	SharedObject target = _room->FindClosestMonster(GetCellPos(), _search_range);	
	if (target == nullptr)
	{
		_target = nullptr;
		return;
	}

	_target = target;
	SetState(EntityState::MOVING);
}

void DummyPlayer::UpdateMoving()
{
	if (_move_tick > GetTickCount64())
		return;
	_move_tick = GetTickCount64() + static_cast<int64>(1000 / 5);

	// TODO:? target nullptr check
	if (_target->GetRoom() != _room)
	{
		StopTargeting();
		BroadcastState();
		return;
	}

	Vector2Int diff = _target->GetCellPos() - GetCellPos();
	if (diff.SimpleDistance == 0 || diff.SimpleDistance > _chase_range)
	{
		StopTargeting();
		BroadcastState();
		return;
	}

	const auto& path = _room->GetMap()->FindPath(GetCellPos(), _target->GetCellPos());
	if (path.size() < 1 || path.size() > _chase_range)
	{
		StopTargeting();
		BroadcastState();
		return;
	}

	if (diff.SimpleDistance <= _skill_range && (diff.x == 0 || diff.y == 0))
	{
		// TODO : set monster level
		_skill_data = g_data_manager->Skill(1);
		_skill_tick = 0;
		SetState(EntityState::SKILL);
		return;
	}

	// move
	SetDir(GetDirFromVector2Int(path[0] - GetCellPos()));
	_room->GetMap()->ApplyMove(shared_from_this(), path[0]);
	BroadcastState();
}

void DummyPlayer::UpdateSkill()
{
	if (_skill_tick == 0)
	{
		// TODO:? target nullptr check
		if (_target->GetRoom() != _room)
		{
			StopTargeting();
			BroadcastState();
			return;
		}

		// can use skill
		Vector2Int diff = _target->GetCellPos() - GetCellPos();
		bool can_use = diff.SimpleDistance <= _skill_range && (diff.x == 0 || diff.y == 0);
		if (can_use == false)
		{
			StopTargeting();
			BroadcastState();
			return;
		}

		MoveDir dir = GetDirFromVector2Int(diff);
		if (dir != GetDir())
		{
			SetDir(dir);
			BroadcastState();
		}

		// damage
		_target->OnDamaged(shared_from_this(), GetStatInfo().attack() + _skill_data->damage);

		// skill broadcast
		{
			S_Skill skill;
			skill.set_objectid(GetObjectId());
			SkillInfo* skill_info = skill.mutable_info();
			skill_info->set_skillid(_skill_data->id);
			_room->Broadcast(GetCellPos(), ClientPacketHandler::MakeSendBuffer(skill));
		}

		// skill cooltime
		int64 skill_tick = static_cast<int64>(1000 * _skill_data->coolDown);
		_skill_tick = GetTickCount64() + skill_tick;
	}
	else if (_skill_tick <= GetTickCount64())
		_skill_tick = 0;
}

void DummyPlayer::UpdateDead()
{
}

void DummyPlayer::BroadcastState()
{
	S_Move move;
	move.set_objectid(GetObjectId());
	PositionInfo* pos = move.mutable_posinfo();
	pos->MergeFrom(GetPositionInfo());

	_room->Broadcast(GetCellPos(), ClientPacketHandler::MakeSendBuffer(move));
}

void DummyPlayer::StopTargeting()
{
	SetState(EntityState::IDLE);
	_target = nullptr;
}
