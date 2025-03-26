#pragma once
#include "Protocol.pb.h"

class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject();

	int32 Id() const { return _object_info.objectid(); }
	void Id(int32 id) { _object_info.set_objectid(id); }

	Protocol::ObjectInfo& GetObjectInfo() { return _object_info; }
	Protocol::StatInfo& GetStatInfo() { return _stat_info; }	

	Protocol::GameObjectType GetType() const { return _object_type; }

	void SetCellPos(Vector2Int cell_pos);
	Vector2Int GetCellPos();

	Protocol::PositionInfo& GetPositionInfo();

protected:
	Protocol::GameObjectType _object_type = Protocol::GameObjectType::NONE;
	Protocol::ObjectInfo _object_info;
	Protocol::StatInfo _stat_info;
	Protocol::PositionInfo _pos_info;
};

