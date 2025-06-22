#include "pch.h"
#include "ViewCube.h"
#include "Monster.h"
#include "Player.h"
#include "Projectile.h"
#include "Room.h"
#include "Zone.h"
#include "ClientPacketHandler.h"
#include "GameSession.h"

ViewCube::ViewCube(SharedPlayer player)
	:_owner(player), IsReset(false)
{	
}

void ViewCube::Update()
{
	SharedRoom room = _owner->GetRoom();
	if (_owner == nullptr || room == nullptr)
	{
		return;
	}

	const xhash_set<SharedObject> current_objects = GetObjects();
	
	xhash_set<SharedObject> prev_objects;

	{
		WRITE_LOCK;
		prev_objects.swap(_prev_objects);
	}

	// Spawn object	
	const auto added = Except(current_objects, prev_objects);
	if (added.size() > 0)
	{
		S_Spawn spawn_packet;
		for (auto& item : added)
		{
			ObjectInfo* object_info = spawn_packet.add_objects();
			object_info->MergeFrom(item->GetObjectInfo());
		}
		_owner->OwnerSession->Send(ClientPacketHandler::MakeSendBuffer(spawn_packet));
	}

	// Despawn object
	const auto vanished = Except(prev_objects, current_objects);
	if (vanished.size() > 0)
	{
		S_Despawn despawn_packet;
		for (auto& item : vanished)
			despawn_packet.add_objectids(item->GetObjectId());

		_owner->OwnerSession->Send(ClientPacketHandler::MakeSendBuffer(despawn_packet));
	}


	{
		WRITE_LOCK;
		for (auto& item : current_objects)
			_prev_objects.insert(item);
	}

	JobReserved = room->PushJobTimerAsync(100, [this](){ Update();});
}

const xhash_set<SharedObject> ViewCube::GetObjects()
{
	SharedRoom room = _owner->GetRoom();
	if (_owner == nullptr || room == nullptr)
	{
		return {};
	}

	if (IsReset)
	{
		WRITE_LOCK;
		_prev_objects.clear();
		IsReset = false;
	}

	auto zones = room->GetAdjacentZones(_owner->GetCellPos(), kViewRange);
	xhash_set<SharedObject> objects;

	// loop only during game objects count
	for (auto& zone : zones)
	{
		auto& players = zone->GetPlayers();
		for (auto player : players)
		{
			if (player == nullptr)
				continue;

			Vector2Int dist = player->GetCellPos() - _owner->GetCellPos();
			if (abs(dist.x) > kViewRange)
				continue;
			if (abs(dist.y) > kViewRange)
				continue;

			objects.insert(move(player));
		}

		auto& monsters = zone->GetMonsters();
		for (auto monster : monsters)
		{
			if (monster == nullptr)
				continue;

			Vector2Int dist = monster->GetCellPos() - _owner->GetCellPos();
			if (abs(dist.x) > kViewRange)
				continue;
			if (abs(dist.y) > kViewRange)
				continue;

			objects.insert(move(monster));
		}

		auto& projectiles = zone->GetProjectiles();
		for (auto projectile : projectiles)
		{
			if (projectile == nullptr)
				continue;

			Vector2Int dist = projectile->GetCellPos() - _owner->GetCellPos();
			if (abs(dist.x) > kViewRange)
				continue;
			if (abs(dist.y) > kViewRange)
				continue;

			objects.insert(move(projectile));
		}
	}

	return objects;
}

const xvector<SharedObject> ViewCube::Except(const xhash_set<SharedObject>& lhs, const xhash_set<SharedObject>& rhs)
{
	xvector<SharedObject> excepts;

	for (auto& object : lhs)
	{
		if (object == nullptr)
			continue;

		auto iter = rhs.find(object);
		if (iter == rhs.end())
			excepts.push_back(object);
	}

	return excepts;
}
