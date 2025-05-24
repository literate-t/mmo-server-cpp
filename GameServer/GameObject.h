#pragma once
#include "Protocol.pb.h"
using namespace Protocol;

struct StatData;
class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject();
	GameObject(Protocol::GameObjectType type);

	int32 GetObjectId() const { return _object_info.objectid(); }
	void SetObjectId(int32 id) { _object_info.set_objectid(id); }

	Protocol::ObjectInfo& GetObjectInfo();
	Protocol::StatInfo& GetStatInfo();
	void SetStatInfoWithStatData(StatData& stat_data);

	void SetObjectName(string_view name);
	string_view GetObjectName();

	Protocol::GameObjectType GetType() const { return _object_type; }

	Protocol::PrefabType GetPrfebType() const;
	void SetPrefabType(Protocol::PrefabType prefab_type);

	void SetCellPos(Vector2Int cell_pos);
	Vector2Int GetCellPos();

	Vector2Int GetFrontCellPosition();
	Vector2Int GetFrontCellPosition(Protocol::MoveDir dir);

	Protocol::PositionInfo& GetPositionInfo();

	void SetRoom(SharedRoom room) { _room = room; }
	SharedRoom GetRoom() { return _room.lock(); }

	int32 GetMaxHp() const;

	int32 GetHp() const;
	void SetHp(int32 hp);
	void AddHp(int32 hp);

	float GetSpeed() const;
	void SetSpeed(float speed);

	void SetState(EntityState state);
	EntityState GetState() const;

	void SetDir(MoveDir dir);
	MoveDir GetDir() const;

	void SetObjectName(string name);

	virtual void Update();
	virtual SharedObject GetOwner();

	virtual void OnDamaged(SharedObject attacker, int32 damage);
	virtual void OnDead(SharedObject attacker);
	virtual void OnDeadAnim();

	virtual int32 GetTotalDefence();
	virtual int32 GetTotalAttack();

	MoveDir GetDirFromVector2Int(Vector2Int diff);

protected:
	Protocol::GameObjectType _object_type = Protocol::GameObjectType::NONE;
	Protocol::ObjectInfo _object_info;
	Protocol::StatInfo* _stat_info;
	Protocol::PositionInfo* _pos_info;

	weak_ptr<class Room> _room;
};

