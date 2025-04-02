#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"
#include "ObjectManager.h"
#include "Map.h"
#include "Zone.h"
#include "ClientPacketHandler.h"
#include "ViewCube.h"

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
}

void Room::Enter(SharedObject object)
{
	if (object == nullptr)
		return;

	// TODO: random position

	GameObjectType type = g_object_manager->GetObjectTypeById(object->GetObjectId());

	if (type == GameObjectType::PLAYER)
	{
		SharedPlayer player = static_pointer_cast<Player>(object);
		_players[player->PlayerDbId] = player;
		
		player->SetRoom(static_pointer_cast<Room>(shared_from_this()));
		player->RefreshStat();

		g_map->ApplyMove(player, Vector2Int(player->GetPositionInfo().posx(), player->GetPositionInfo().posy()));
		GetZone(player->GetCellPos())->GetPlayers().insert(player);

		// 접속 본인 정보 전송
		{
			S_EnterGame enter_game;
			enter_game.set_allocated_player(&player->GetObjectInfo());
			player->OwnerSession->Send(ClientPacketHandler::MakeSendBuffer(enter_game));
		}

		player->GetView()->IsReset = true;
		player->GetView()->Update();
	}
}

void Room::Leave(SharedObject player)
{
	//_players.erase(player->PlayerDbId);
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

Vector2Int Room::GetZoneIndex(Vector2Int cell_pos)
{
	int32 index_x = (cell_pos.x - _map->GetMinX()) / _zone_cell_size;
	int32 index_y = (_map->GetMaxY() - cell_pos.y) / _zone_cell_size;

	if (index_x < 0 || index_x >= _zones[0].size()) return Vector2Int();
	if (index_y < 0 || index_y >= _zones.size()) return Vector2Int();

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
		if (path_list.size() < 2 || path_list.size() > range)
			continue;

		return player;
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

	if ((player_info.posinfo().posx() == move_pos_info.posx()) && (player_info.posinfo().posy() == move_pos_info.posy()))
		return;
	
	Vector2Int move_pos = Vector2Int(move_pos_info.posx(), move_pos_info.posy());
	if (!_map->CanGo(move_pos))
		return;

	// update my state
	player->GetPositionInfo().set_state(move_pos_info.state());
	player->GetPositionInfo().set_movedir(move_pos_info.movedir());
	_map->ApplyMove(player, move_pos);

	// broacast to others	
	S_Move res_move_packet;
	res_move_packet.set_objectid(player->GetObjectId());
	Protocol::PositionInfo pos(move_packet.posinfo());
	res_move_packet.set_allocated_posinfo(&pos);

	Broadcast(player->GetCellPos(), ClientPacketHandler::MakeSendBuffer(res_move_packet));
}