#pragma once
#include "JobQueue.h"

class Room : public JobQueue
{
public:
	Room();
	
	void Init(int32 map_id, int32 zone_cell_size);

	void Enter(SharedObject object, bool random_pos = true);
	void Leave(int32 object_id);
	void Broadcast(SharedSendBuffer send_buffer);
	void Broadcast(Vector2Int pos, SharedSendBuffer send_buffer);

	// Zone
	SharedZone GetZone(Vector2Int cell_pos);
	SharedZone GetZone(int32 index_x, int32 index_y);
	optional<Vector2Int> GetZoneIndex(Vector2Int cell_pos);

	int16 RoomId = 0;

	const SharedMap& GetMap() const { return _map; }

	xvector<SharedZone> GetAdjacentZones(Vector2Int cell_pos, int32 range = kRange);
	xvector<SharedPlayer> GetAdjacentPlayers(Vector2Int cell_pos, int32 range = kRange);
	xvector<SharedMonster> GetAdjacentMonsters(Vector2Int cell_pos, int32 range = kRange);
	xvector<SharedProjectile> GetAdjacentProjectiles(Vector2Int cell_pos, int32 range = kRange);

	SharedMonster GetMonster(int32 object_id);
	SharedPlayer GetPlayer(int32 object_id);

	SharedPlayer FindClosestPlayer(Vector2Int base_pos, int32 range);
	SharedMonster FindClosestMonster(Vector2Int base_pos, int32 range);
	SharedPlayer FindPlayer(function<bool(SharedPlayer)> predicate);

	// handle packet
	void HandleMovePacket(SharedPlayer player, const Protocol::C_Move& move_packet);
	void HandleSkillPacket(SharedPlayer player, const Protocol::C_Skill& skill_packet);
	void HandleEquipItemPacket(SharedPlayer player, const Protocol::C_EquipItem& equip_packet);
	void HandleUseItemPacket(SharedPlayer player, const Protocol::C_UseItem& use_packet);
	void HandleDropItemPacket(SharedPlayer player, const Protocol::C_DropItem& drop_packet);
	void HandleGoRandomPacket(SharedPlayer player, const Protocol::C_GoRandom& random_packet);

	Vector2Int GetRandomPos();

public:
	static constexpr int32 kRange = 10;

private:
	xmap<const uint64, SharedPlayer> _players;	
	xmap<const uint64, SharedMonster> _monsters;
	xmap<const uint64, SharedProjectile> _projectiles;

	SharedMap _map;
	int32 _zone_cell_size;
	xvector<xvector<SharedZone>> _zones;
};