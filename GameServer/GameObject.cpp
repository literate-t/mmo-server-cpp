#include "pch.h"
#include "GameObject.h"
using namespace Protocol;
#include "Room.h"
#include "ClientPacketHandler.h"

GameObject::GameObject()
	:_pos_info(_object_info.mutable_posinfo()),
	_stat_info(_object_info.mutable_statinfo()),
	_object_type(Protocol::GameObjectType::NONE)
{
}

GameObject::GameObject(Protocol::GameObjectType type)
	:_pos_info(_object_info.mutable_posinfo()),
	_stat_info(_object_info.mutable_statinfo()),
	_object_type(type)
{
}

Protocol::ObjectInfo& GameObject::GetObjectInfo()
{
	return _object_info;
}

Protocol::StatInfo& GameObject::GetStatInfo()
{
	return *_stat_info;
}

void GameObject::SetCellPos(Vector2Int cell_pos)
{
	_pos_info->set_posx(cell_pos.x);
	_pos_info->set_posy(cell_pos.y);
}

Protocol::PositionInfo& GameObject::GetPositionInfo()
{
	return *_pos_info;
}

int32 GameObject::GetHp() const
{	
	return _stat_info->hp();
}

void GameObject::SetHp(int32 hp)
{
	_stat_info->set_hp(hp);
}

float GameObject::GetSpeed() const
{
	return _stat_info->speed();
}

void GameObject::SetSpeed(float speed)
{
	_stat_info->set_speed(speed);
}

void GameObject::SetState(EntityState state)
{
	_pos_info->set_state(state);
}

EntityState GameObject::GetState() const
{
	return _pos_info->state();
}

void GameObject::SetDir(MoveDir dir)
{
	_pos_info->set_movedir(dir);
}

MoveDir GameObject::GetDir() const
{
	return _pos_info->movedir();
}

void GameObject::SetObjectName(string name)
{
	_object_info.set_name(name);
}

void GameObject::Update()
{
}

SharedObject GameObject::GetOwner()
{
	return shared_from_this();
}

void GameObject::OnDamaged(SharedObject attacker, int32 damage)
{
	auto room = GetRoom();
	if (room == nullptr) return;

	int32 final_damage = max(0, damage - GetTotalDefence());
	SetHp(max(0, GetHp() - final_damage));

	S_ChangeHp change_hp;
	change_hp.set_objectid(GetObjectId());
	change_hp.set_hp(GetHp());
	room->Broadcast(GetCellPos(), ClientPacketHandler::MakeSendBuffer(change_hp));

	if (GetHp() == 0)
		OnDead(attacker);
}

void GameObject::OnDead(SharedObject attacker)
{
	auto room = GetRoom();
	if (room == nullptr) return;

	S_Die die_packet;
	die_packet.set_attackerid(GetObjectId());
	// can be necessary in client
	die_packet.set_attackerid(attacker->GetObjectId());

	room->Broadcast(GetCellPos(), ClientPacketHandler::MakeSendBuffer(die_packet));

	room->Leave(GetObjectId());

	SetHp(GetStatInfo().maxhp());
	GetPositionInfo().set_state(EntityState::IDLE);
	GetPositionInfo().set_movedir(MoveDir::DOWN);
	SetCellPos(Vector2Int(0, 0));

	room->Enter(shared_from_this());
}

int32 GameObject::GetTotalDefence()
{
	return int32();
}

int32 GameObject::GetTotalAttack()
{
	return int32();
}

MoveDir GameObject::GetDirFromVector2Int(Vector2Int diff)
{
	if (diff.x < 0)
		return MoveDir::LEFT;
	else if (diff.x > 0)
		return MoveDir::RIGHT;
	else if (diff.y < 0)
		return MoveDir::DOWN;
	else
		return MoveDir::UP;	
}

Vector2Int GameObject::GetCellPos()
{
	return Vector2Int(_pos_info->posx(), _pos_info->posy());
}

Vector2Int GameObject::GetFrontCellPosition()
{
	return GetFrontCellPosition(_pos_info->movedir());
}

Vector2Int GameObject::GetFrontCellPosition(Protocol::MoveDir dir)
{
	Vector2Int cell_pos = GetCellPos();

	switch (dir)
	{
		case MoveDir::UP:
			cell_pos += Vector2Int::Up();
			break;
		case MoveDir::DOWN:
			cell_pos += Vector2Int::Down();
			break;
		case MoveDir::LEFT:
			cell_pos += Vector2Int::Left();
			break;
		case MoveDir::RIGHT:
			cell_pos += Vector2Int::Right();
			break;
	}

	return cell_pos;
}
