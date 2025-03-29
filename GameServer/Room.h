#pragma once
#include "JobQueue.h"

class Room : public JobQueue
{
public:
	Room();
	
	void Init(int32 map_id, int32 zone_cell_size);

	void Enter(SharedObject object);
	void Leave(SharedObject object);
	void Broatcast(SharedSendBuffer send_buffer);

	// Zone
	SharedZone GetZone(Vector2Int cell_pos);
	SharedZone GetZone(int32 index_x, int32 index_y);
	Vector2Int GetZoneIndex(Vector2Int cell_pos);

	int16 RoomId = 0;

	const SharedMap& GetMap() const { return _map; }

	const xvector<SharedPlayer>& GetAdjacentPlayers(Vector2Int cell_pos, int32 range);
	const xvector<SharedZone>& GetAdjacentZones(Vector2Int cell_pos, int32 range);

	SharedPlayer FindClosestPlayer(Vector2Int base_pos, int32 range);
	SharedPlayer FindPlayer(function<bool(SharedPlayer)> predicate);

private:
	xmap<const uint64, SharedPlayer> _players;	
	SharedMap _map;
	int32 _zone_cell_size;
	xvector<xvector<SharedZone>> _zones;
	xvector<SharedZone> _adjacent_zones;
	xvector<SharedPlayer> _adjacent_players;
};