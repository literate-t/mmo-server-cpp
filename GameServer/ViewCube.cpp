#include "pch.h"
#include "ViewCube.h"

ViewCube::ViewCube(SharedPlayer player)
	:_owner(player), IsReset(false)
{	
}

void ViewCube::Update()
{
	SharedRoom room = _owner->GetRoom();
	if (_owner == nullptr || room == nullptr)
	{
		_objects.clear();
		return;
	}

	const xhash_set<SharedObject>& current_objects = GetObjects();

	// Spawn object
	const auto& added = Except(current_objects, _prev_objects);
	if (added.size() > 0)
	{
		S_Spawn spawn_packet;
		for (const auto& item : added)
		{
			ObjectInfo* object_info = spawn_packet.add_objects();
			object_info->MergeFrom(item->GetObjectInfo());
		}
		_owner->OwnerSession->Send(ClientPacketHandler::MakeSendBuffer(spawn_packet));
	}

	// Despawn object
	const auto& vanished = Except(_prev_objects, current_objects);
	if (vanished.size() > 0)
	{
		S_Despawn despawn_packet;
		for (const auto& item : vanished)
			despawn_packet.add_objectids(item->GetObjectId());

		_owner->OwnerSession->Send(ClientPacketHandler::MakeSendBuffer(despawn_packet));
}

	_prev_objects.clear();
	for (const auto& item : current_objects)
		_prev_objects.insert(item);

	room->PushTimerAsync(100, [this](){ Update();});
}
{

	return _objects;
}
