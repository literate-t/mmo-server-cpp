#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
{
	_pos_info = _object_info.mutable_posinfo();
	_stat_info = _object_info.mutable_statinfo();
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

Vector2Int GameObject::GetCellPos()
{
	return Vector2Int(_pos_info->posx(), _pos_info->posy());
}
