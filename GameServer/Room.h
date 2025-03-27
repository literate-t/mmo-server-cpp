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

	int16 RoomId = 0;

	const SharedMap& GetMap() const { return _map; }

private:
	xmap<const uint64, SharedPlayer> _players;	
	SharedMap _map;
};