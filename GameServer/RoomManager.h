#pragma once
class RoomManager
{
public:
	SharedRoom Add(int32 map_id);
	SharedRoom Find(int32 room_id);
	bool Remove(int32 room_id);

private:
	xmap<int16, SharedRoom> _map_table;
	int16 _id = 1;

	USE_LOCK;
};

