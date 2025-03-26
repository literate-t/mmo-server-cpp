#pragma once
#include "Protocol.pb.h"

class GameObject
{
public:
	GameObject() = default;
	~GameObject() = default;

	int32 Id() const { return _id; }
	void Id(int32 id) { _id = id; }

	Protocol::ObjectInfo& GetObjectInfo() { return _object_info; }
	Protocol::StatInfo& GetStatInfo() { return _stat_info; }	

	Protocol::GameObjectType GetType() const { return _object_type; }

protected:
	Protocol::GameObjectType _object_type = Protocol::GameObjectType::NONE;
	int32 _id;
	Protocol::ObjectInfo _object_info;
	Protocol::StatInfo _stat_info;
};

