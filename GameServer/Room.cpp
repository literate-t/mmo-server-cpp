#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "DummyPlayer.h"
#include "GameSession.h"
#include "ObjectManager.h"
#include "DataManager.h"
#include "Map.h"
#include "Zone.h"
#include "ClientPacketHandler.h"
#include "ViewCube.h"
#include "Arrow.h"
#include "Monster.h"
#include <random>

Room::Room()
	:_map(MakeShared<Map>()), _zone_cell_size(0)
{
}

void Room::Init(int32 map_id, int32 zone_cell_size)
{
	_map->LoadMap(map_id);
	_zone_cell_size = zone_cell_size;

	int32 count_x = (_map->GetSizeX() + zone_cell_size - 1) / zone_cell_size;
	int32 count_y = (_map->GetSizeY() + zone_cell_size - 1) / zone_cell_size;

	// set zones
	_zones.resize(count_y);
	for (int y = 0; y < count_y; ++y)
	{
		_zones[y].resize(count_x);
		for (int x = 0; x < count_x; ++x)
			_zones[y][x] = MakeShared<Zone>(y, x);
	}

	// monsters
	for (int i = 0; i < 10; ++i)
	{
		SharedMonster monster = g_object_manager->Add<Monster>();
		monster->Init(1);
		PushJobAsync(&Room::Enter, monster, true);
	}
}

void Room::Enter(SharedObject object, bool random_pos)
{
	if (object == nullptr)
		return;

	// random position
	if (random_pos)
		object->SetCellPos(GetRandomPos());

	int32 object_id = object->GetObjectId();
	GameObjectType type = g_object_manager->GetObjectTypeById(object_id);

	Vector2Int pos_info = object->GetCellPos();

	if (type == GameObjectType::PLAYER)
	{
		SharedPlayer player = static_pointer_cast<Player>(object);
		_players[object_id] = player;
		
		player->SetRoom(static_pointer_cast<Room>(shared_from_this()));
		player->RefreshStat();

		_map->ApplyMove(player, Vector2Int(pos_info.x, pos_info.y));
		GetZone(pos_info)->GetPlayers().insert(player);

		// 접속 본인 정보 전송
		{
			S_EnterGame enter_game;
			ObjectInfo* info = enter_game.mutable_player();
			info->MergeFrom(player->GetObjectInfo());
			player->OwnerSession->Send(ClientPacketHandler::MakeSendBuffer(enter_game));
		}

		if (player->IsDummy() == false)
		{
		player->MakeViewCube();
		player->GetView()->IsReset = true;
		player->GetView()->Update();
	}
		else
			player->Update();
	}
	else if (type == GameObjectType::PROJECTILE)
	{
		SharedProjectile projectile = static_pointer_cast<Projectile>(object);
		projectile->SetRoom(static_pointer_cast<Room>(shared_from_this()));
		GetZone(pos_info)->GetProjectiles().insert(projectile);
		projectile->Update();
		_projectiles[object_id] = projectile;
	}
	else if (type == GameObjectType::MONSTER)
	{
		SharedMonster monster = static_pointer_cast<Monster>(object);
		monster->SetRoom(static_pointer_cast<Room>(shared_from_this()));
		GetZone(pos_info)->GetMonsters().insert(monster);
		_map->ApplyMove(monster, monster->GetCellPos());
		monster->SetObjectName(to_string(object_id));
		monster->Update();
		_monsters[object_id] = monster;
	}

	// spwan info
	S_Spawn spawn;
	ObjectInfo* new_object = spawn.add_objects();
	new_object->MergeFrom(object->GetObjectInfo());
	Broadcast(pos_info, ClientPacketHandler::MakeSendBuffer(spawn));
}

void Room::Leave(int32 object_id)
{
	GameObjectType type =  g_object_manager->GetObjectTypeById(object_id);
	Vector2Int pos;

	if (type == GameObjectType::PLAYER)
	{
		SharedPlayer player = _players[object_id];
		if (0 == _players.erase(object_id))
			return;

		player->OnLeaveGame();

		pos = player->GetCellPos();

		_map->ApplyLeave(player);

		if (player->IsDummy() == false)
		{
		player->GetView()->JobReserved->Cancel();
		player->ReleaseViewCube();
		}

		// to me
		S_LeaveGame leave;
		player->OwnerSession->Send(ClientPacketHandler::MakeSendBuffer(leave));
	}
	else if (type == GameObjectType::MONSTER)
	{
		SharedMonster monster = static_pointer_cast<Monster>(_monsters[object_id]);
		if (0 == _monsters.erase(object_id))
			return;

		pos = monster->GetCellPos();
		_map->ApplyLeave(monster);
		monster->SetRoom(nullptr);
	}
	else if (type == GameObjectType::PROJECTILE)
	{
		SharedArrow arrow = static_pointer_cast<Arrow>(_projectiles[object_id]);
		if (0 == _projectiles.erase(object_id))
			return;

		pos = arrow->GetCellPos();
		_map->ApplyLeave(arrow);
		arrow->SetRoom(nullptr);
		arrow->Owner = nullptr;
	}

	// to others
	S_Despawn despawn;
	despawn.add_objectids(object_id);
	Broadcast(pos, ClientPacketHandler::MakeSendBuffer(despawn));
}

void Room::Broadcast(SharedSendBuffer send_buffer)
{
	for (auto& p : _players)
	{
		p.second->OwnerSession->Send(send_buffer);
	}
}

void Room::Broadcast(Vector2Int pos, SharedSendBuffer send_buffer)
{
	const xvector<SharedPlayer> players = GetAdjacentPlayers(pos);
	for (auto& player : players)
	{
		int32 dist_x = abs(pos.x - player->GetCellPos().x);
		int32 dist_y = abs(pos.y - player->GetCellPos().y);

		if (dist_x <= kRange && dist_y <= kRange)
			player->OwnerSession->Send(send_buffer);
	}
}

SharedZone Room::GetZone(Vector2Int cell_pos)
{	
	int32 index_x = (cell_pos.x - _map->GetMinX()) / _zone_cell_size;
	int32 index_y = (_map->GetMaxY() - cell_pos.y) / _zone_cell_size;

	return GetZone(index_x, index_y);
}

SharedZone Room::GetZone(int32 index_x, int32 index_y)
{
	if (index_x < 0 || index_x >= _zones[0].size()) return nullptr;
	if (index_y < 0 || index_y >= _zones.size()) return nullptr;
	
	return _zones[index_y][index_x];
}

optional<Vector2Int> Room::GetZoneIndex(Vector2Int cell_pos)
{
	int32 index_x = (cell_pos.x - _map->GetMinX()) / _zone_cell_size;
	int32 index_y = (_map->GetMaxY() - cell_pos.y) / _zone_cell_size;

	if (index_x < 0 || index_x >= _zones[0].size()) return nullopt;
	if (index_y < 0 || index_y >= _zones.size()) return nullopt;

	return Vector2Int(index_x, index_y);
}

// TODO: proxy pattern
const xvector<SharedPlayer>& Room::GetAdjacentPlayers(Vector2Int cell_pos, int32 range)
{
	const xvector<SharedZone>& zones = GetAdjacentZones(cell_pos, range);

	_adjacent_players.clear();
	size_t total_size = 0;

	for (auto& zone : zones)
		total_size += zone->GetPlayers().size();

	_adjacent_players.reserve(total_size);

	for (auto& zone : zones)
		_adjacent_players.insert(_adjacent_players.end(), zone->GetPlayers().begin(), zone->GetPlayers().end());

	return _adjacent_players;
}

const xvector<SharedMonster>& Room::GetAdjacentMonsters(Vector2Int cell_pos, int32 range)
{
	const xvector<SharedZone>& zones = GetAdjacentZones(cell_pos, range);

	_adjacent_monsters.clear();
	size_t total_size = 0;

	for (auto& zone : zones)
		total_size += zone->GetMonsters().size();

	_adjacent_monsters.reserve(total_size);

	for (auto& zone : zones)
		_adjacent_monsters.insert(_adjacent_monsters.end(), zone->GetMonsters().begin(), zone->GetMonsters().end());

	return _adjacent_monsters;
}

const xvector<SharedProjectile>& Room::GetAdjacentProjectiles(Vector2Int cell_pos, int32 range)
{
	const xvector<SharedZone>& zones = GetAdjacentZones(cell_pos, range);

	_adjacent_projectiles.clear();
	size_t total_size = 0;

	for (auto& zone : zones)
		total_size += zone->GetProjectiles().size();

	_adjacent_projectiles.reserve(total_size);

	for (auto& zone : zones)
		_adjacent_projectiles.insert(_adjacent_projectiles.end(), zone->GetProjectiles().begin(), zone->GetProjectiles().end());

	return _adjacent_projectiles;
}

// TODO: proxy pattern
const xvector<SharedZone>& Room::GetAdjacentZones(Vector2Int cell_pos, int32 range)
{
	_adjacent_zones.clear();

	int32 min_x = cell_pos.x - range;
	int32 min_y = cell_pos.y - range;
	int32 max_x = cell_pos.x + range;
	int32 max_y = cell_pos.y + range;

	Vector2Int left_top = Vector2Int(min_x, max_y);
	Vector2Int right_bottom = Vector2Int(max_x, min_y);

	int32 left_top_x = (left_top.x - _map->GetMinX()) / _zone_cell_size;
	int32 left_top_y = (_map->GetMaxY() - left_top.y) / _zone_cell_size;
	int32 right_bottom_x = (right_bottom.x - _map->GetMinX()) / _zone_cell_size;
	int32 right_bottom_y = (_map->GetMaxY() - right_bottom.y) / _zone_cell_size;

	for (int32 y = left_top_y; y <= right_bottom_y; ++y)
	{
		for (int32 x = left_top_x; x <= right_bottom_x; ++x)
		{
			SharedZone zone = GetZone(x, y);
			if (zone == nullptr)
				continue;

			_adjacent_zones.push_back(zone);
		}
	}

	return _adjacent_zones;
}

SharedPlayer Room::FindClosestPlayer(Vector2Int base_pos, int32 range)
{
	xvector<SharedPlayer> players = GetAdjacentPlayers(base_pos, range);
	if (players.empty())
		return nullptr;
	
	sort(players.begin(), players.end(), [&base_pos](const SharedPlayer& a, const SharedPlayer& b)
		{
			int32 left_dist = (base_pos - a->GetCellPos()).SquareMagnitude;
			int32 right_dist = (base_pos - b->GetCellPos()).SquareMagnitude;

			return left_dist < right_dist;
		});

	// Even if simple distance exists
	// it may not be accessible due to obstacles
	for (auto& player : players)
	{
		auto& path_list = _map->FindPath(base_pos, player->GetCellPos(), false, range);
		if (path_list.size() < 1 || path_list.size() > range)
			continue;

		return player;
	}

	return nullptr;
}

SharedMonster Room::FindClosestMonster(Vector2Int base_pos, int32 range)
{
	xvector<SharedMonster> monsters = GetAdjacentMonsters(base_pos, range);
	if (monsters.empty())
		return nullptr;

	sort(monsters.begin(), monsters.end(), [&base_pos](const SharedMonster& a, const SharedMonster& b)
		{
			int32 left_dist = (base_pos - a->GetCellPos()).SquareMagnitude;
			int32 right_dist = (base_pos - b->GetCellPos()).SquareMagnitude;

			return left_dist < right_dist;
		});

	// Even if simple distance exists
	// it may not be accessible due to obstacles
	for (auto& monster : monsters)
	{
		auto& path_list = _map->FindPath(base_pos, monster->GetCellPos(), false, range);
		if (path_list.size() < 1 || path_list.size() > range)
			continue;

		return monster;
	}

	return nullptr;
}

SharedPlayer Room::FindPlayer(function<bool(SharedPlayer)> predicate)
{
	for (auto& [key, player] : _players)
	{
		if (predicate(player))
			return player;
	}

	return SharedPlayer();
}

void Room::HandleMovePacket(SharedPlayer player, const Protocol::C_Move& move_packet)
{
	const Protocol::PositionInfo& move_pos_info = move_packet.posinfo();
	Protocol::ObjectInfo& player_info = player->GetObjectInfo();

	Vector2Int move_pos = Vector2Int(move_pos_info.posx(), move_pos_info.posy());
	// pass if two values are same
	if ((player_info.posinfo().posx() != move_pos_info.posx()) || (player_info.posinfo().posy() != move_pos_info.posy()))
	{	
		if (!_map->CanGo(move_pos))
			return;
	}

	// update my state
	player->GetPositionInfo().set_state(move_pos_info.state());
	player->GetPositionInfo().set_movedir(move_pos_info.movedir());
	_map->ApplyMove(player, move_pos);

	// broacast to others	
	S_Move res_move_packet;
	res_move_packet.set_objectid(player->GetObjectId());
	PositionInfo* pos = res_move_packet.mutable_posinfo();
	pos->MergeFrom(move_packet.posinfo());

	Broadcast(player->GetCellPos(), ClientPacketHandler::MakeSendBuffer(res_move_packet));
}

void Room::HandleSkillPacket(SharedPlayer player, const Protocol::C_Skill& skill_packet)
{
	PositionInfo& pos_info = player->GetPositionInfo();
	// skill only on idle
	EntityState state = pos_info.state();
	if (pos_info.state() != EntityState::IDLE)
		return;

	pos_info.set_state(EntityState::SKILL);
	S_Skill skill;
	skill.set_objectid(player->GetObjectId());
	int32 skill_id = skill_packet.info().skillid();
	SkillInfo* skill_info = skill.mutable_info();
	skill_info->set_skillid(skill_id);

	// skill and animation
	Broadcast(player->GetCellPos(), ClientPacketHandler::MakeSendBuffer(skill));
	
	SkillData* skill_data = g_data_manager->Skill(skill_id);

	switch (skill_data->skillType)
	{
		case SkillType::SKILL_AUTO:
		{
			// punch attack damage
			Vector2Int front_cell = player->GetFrontCellPosition();
			SharedObject target = _map->Find(front_cell);
			if (nullptr == target)
				return;

			// TODO : hit

			break;
		}
		case SkillType::SKILL_PROJECTILE:
		{
			SharedArrow arrow = g_object_manager->Add<Arrow>();
			{
				arrow->Owner = player;
				arrow->Data = skill_data;
				PositionInfo& arrow_pos = arrow->GetPositionInfo();
				arrow_pos.set_state(EntityState::MOVING);
				arrow_pos.set_movedir(player->GetPositionInfo().movedir());
				arrow_pos.set_posx(player->GetCellPos().x);
				arrow_pos.set_posy(player->GetCellPos().y);
				arrow->SetSpeed(skill_data->projectile.speed);

				PushJobAsync(&Room::Enter, arrow, false);
				break;
			}
		}
	}
}

void Room::HandleEquipItemPacket(SharedPlayer player, const Protocol::C_EquipItem& skill_packet)
{
	player->HandleEquipItemPacket(skill_packet);
}

Vector2Int Room::GetRandomPos()
{
	random_device rd;
	mt19937 gen(rd());

	while (true)
	{
		uniform_int_distribution<> random_dis_x(_map->GetMinX() + 1, _map->GetMaxX() - 1);
		uniform_int_distribution<> random_dis_y(_map->GetMinY() + 1, _map->GetMaxY() - 1);

		int32 random_x = random_dis_x(gen);
		int32 random_y = random_dis_y(gen);

		Vector2Int random_pos(random_x, random_y);
		if (_map->CanGo(random_pos))
			return random_pos;
	}
}
