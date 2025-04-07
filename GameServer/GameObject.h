#pragma once
#include "Protocol.pb.h"

class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject();
	GameObject(Protocol::GameObjectType type);

	int32 GetObjectId() const { return _object_info.objectid(); }
	void SetObjectId(int32 id) { _object_info.set_objectid(id); }

	Protocol::ObjectInfo& GetObjectInfo();
	Protocol::StatInfo& GetStatInfo();

	Protocol::GameObjectType GetType() const { return _object_type; }

	void SetCellPos(Vector2Int cell_pos);
	Vector2Int GetCellPos();

	Vector2Int GetFrontCellPosition();
	Vector2Int GetFrontCellPosition(Protocol::MoveDir dir);

	Protocol::PositionInfo& GetPositionInfo();

	void SetRoom(SharedRoom room) { _room = room; }
	SharedRoom GetRoom() { return _room.lock(); }

protected:
	Protocol::GameObjectType _object_type = Protocol::GameObjectType::NONE;
	Protocol::ObjectInfo _object_info;
	Protocol::StatInfo* _stat_info;
	Protocol::PositionInfo* _pos_info;

	weak_ptr<class Room> _room;
};

