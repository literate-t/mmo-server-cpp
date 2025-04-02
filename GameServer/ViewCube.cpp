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

	JobReserved = room->PushTimerAsync(100, [this](){ Update();});
}

const xhash_set<SharedObject>& ViewCube::GetObjects()
{
	SharedRoom room = _owner->GetRoom();
	if (_owner == nullptr || room == nullptr)
	{
		_objects.clear();
		return _objects;
	}

	if (IsReset)
	{
		_prev_objects.clear();
		IsReset = false;
	}

	const auto& zones = room->GetAdjacentZones(_owner->GetCellPos(), kViewRange);

	// loop only during game objects count
	for (const auto& zone : zones)
	{
		const auto& players = zone->GetPlayers();
		for (auto& player : players)
		{
			Vector2Int dist = player->GetCellPos() - _owner->GetCellPos();
			if (abs(dist.x) > kViewRange)
				continue;
			if (abs(dist.y) > kViewRange)
				continue;

			_objects.insert(player);
		}

		const auto& monsters = zone->GetMonsters();
		for (auto& monster : monsters)
		{
			Vector2Int dist = monster->GetCellPos() - _owner->GetCellPos();
			if (abs(dist.x) > kViewRange)
				continue;
			if (abs(dist.y) > kViewRange)
				continue;

			_objects.insert(monster);
		}

		const auto& projectiles = zone->GetProjectiles();
		for (auto& projectile : projectiles)
		{
			Vector2Int dist = projectile->GetCellPos() - _owner->GetCellPos();
			if (abs(dist.x) > kViewRange)
				continue;
			if (abs(dist.y) > kViewRange)
				continue;

			_objects.insert(projectile);
		}
	}

	return _objects;
}

const xvector<SharedObject>& ViewCube::Except(const xhash_set<SharedObject>& base, const xhash_set<SharedObject>& rhs)
{
	for (SharedObject object : rhs)
	{
		auto iter = base.find(object);
		if (iter != base.end())
			continue;

		_excepts.push_back(object);
	}

	return _excepts;
}
