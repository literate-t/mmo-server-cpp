#include "pch.h"
#include "GameObject.h"

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

int32 GameObject::GetHp()
{
	return _stat_info->hp();
}

void GameObject::SetHp(int32 hp)
{
	_stat_info->set_hp(hp);
}

float GameObject::GetSpeed()
{
	return _stat_info->speed();
}

void GameObject::SetSpeed(float speed)
{
	_stat_info->set_speed(speed);
}

void GameObject::Update()
{
}

SharedObject GameObject::GetOwner()
{
	return shared_from_this();
}
}
int32 GameObject::GetTotalDefence()
{
	return int32();
}

int32 GameObject::GetTotalAttack()
{
	return int32();
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
