#include "pch.h"
#include "RoomManager.h"
#include "Room.h"

SharedRoom RoomManager::Add(int32 map_id)
{
	SharedRoom room = MakeShared<Room>();
	room->PushJobAsync(&Room::Init, map_id, 10);

	WRITE_LOCK;
	room->RoomId = _id;
	_map_table[_id] = room;
	++_id;

	return room;
}

SharedRoom RoomManager::Find(int32 room_id)
{
	return _map_table[room_id];
}

bool RoomManager::Remove(int32 room_id)
{
	WRITE_LOCK;
	size_t erase_count = _map_table.erase(room_id);

	return erase_count > 0;
}


