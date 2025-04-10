#include "pch.h"
#include "Map.h"
#include "GameObject.h"
#include "StringReader.h"
#include "Util.h"
#include "Room.h"
#include "Zone.h"
#include "ObjectManager.h"
#include "Player.h"
#include "Projectile.h"
#include "Protocol.pb.h"

void Map::InitArrays(int32 size_x, int32 size_y)
{
	// _collisions
	_collisions.resize(size_y);
	for (int i = 0; i < _collisions.size(); ++i)
	{
		_collisions[i].resize(size_x);
		for (int j = 0; j < _collisions[i].size(); ++j)
			_collisions[i][j] = false;
	}

	// _objects
	_objects.resize(size_y);
	for (int i = 0; i < _objects.size(); ++i)
	{
		_objects[i].resize(size_x);
		for (int j = 0; j < _objects[i].size(); ++j)
			_objects[i][j] = nullptr;
	}
}

void Map::LoadMap(int32 map_id, string prefix_path)	
{
	string map_name = "Map_" + Util::ToString(map_id, 3);

	StringReader reader(prefix_path + "/" + map_name + ".txt");
	_minx = stoi(reader.ReadLine());
	_maxx = stoi(reader.ReadLine());
	_miny = stoi(reader.ReadLine());
	_maxy = stoi(reader.ReadLine());

	int32 size_x = GetSizeX();
	int32 size_y = GetSizeY();

	InitArrays(size_x, size_y);

	for (int32 y = 0; y < size_y; ++y)
	{
		string line = reader.ReadLine();
		for (int32 x = 0; x < size_x; ++x)
			_collisions[y][x] = line[x] == '1' ? true : false;
	}
}

bool Map::ApplyMove(SharedObject object, Vector2Int dest, bool is_through_objects, bool apply_collision)
{
	if (object->GetRoom() == nullptr) return false;
	if (!CanGo(dest, is_through_objects)) return false;

	Protocol::PositionInfo& pos = object->GetPositionInfo();
	if (IsWithinBounds(pos.posx(), pos.posy()) == false)
		return false;

	// 충돌을 적용하기 위해서는 _objects 배열에 정보를 넣어야 한다
	if (apply_collision)
	{
		Vector2Int now = GetCoordIndex(object->GetCellPos());
		if (_objects[now.y][now.x] == object)
			_objects[now.y][now.x] = nullptr;

		Vector2Int next = GetCoordIndex(dest);
		_objects[next.y][next.x] = object;
	}

	// 현재 위치에 대한 Zone 처리
	SetCurrentZone(object, dest);

	object->SetCellPos(dest);

	return true;
}

bool Map::ApplyLeave(SharedObject object)
{
	if (object->GetRoom() == nullptr) return false;

	SharedRoom room = object->GetRoom();
	Vector2Int cell_pos = object->GetCellPos();
	
	if (!IsWithinBounds(cell_pos.x, cell_pos.y))
		return false;

	room->GetZone(cell_pos)->Remove(object);

	Vector2Int index_pos = GetCoordIndex(cell_pos);
	if (_objects[index_pos.y][index_pos.x] == object)
		_objects[index_pos.y][index_pos.x] = nullptr;

	return true;
}

bool Map::CanGo(Vector2Int cell_pos, bool is_through_objects)
{
	if (cell_pos.x < _minx || cell_pos.x > _maxx) return false;
	if (cell_pos.y < _miny || cell_pos.y > _maxy) return false;

	Vector2Int index_pos = GetCoordIndex(cell_pos);
	
	return !HasCollision(index_pos, is_through_objects);
}

bool Map::HasCollision(Vector2Int index_pos, bool is_through_objects)
{	
	return _collisions[index_pos.y][index_pos.x] || (!is_through_objects && _objects[index_pos.y][index_pos.x] != nullptr);
}

bool Map::IsWithinBounds(int32 x, int32 y)
{
	if (x < _minx || x > _maxx || y < _miny || y > _maxy) return false;
	return true;
}

void Map::SetCurrentZone(SharedObject object, Vector2Int dest)
{
	auto room = object->GetRoom();
	if (room)
	{		
		SharedZone before = room->GetZone(object->GetCellPos());
		SharedZone after = room->GetZone(dest);

		if (before != after)
		{
			GameObjectType type = g_object_manager->GetObjectTypeById(object->GetObjectId());
			switch (type)
			{
				case GameObjectType::PLAYER:
				{
					SharedPlayer player = static_pointer_cast<Player>(object);
					before->GetPlayers().erase(player);
					after->GetPlayers().insert(player);
					break;
				}	
				case GameObjectType::MONSTER:
				{
					SharedMonster monster = static_pointer_cast<Monster>(object);
					before->GetMonsters().erase(monster);
					after->GetMonsters().insert(monster);
					break;
				}
				case GameObjectType::PROJECTILE:
				{
					SharedProjectile arrow = static_pointer_cast<Projectile>(object);
					before->GetProjectiles().erase(arrow);
					after->GetProjectiles().insert(arrow);
					break;
				}
			}
		}
	}
}

Vector2Int Map::GetCoordIndex(Vector2Int cell_pos)
{
	return GetCoordIndex(cell_pos.x, cell_pos.y);
}

Vector2Int Map::GetCoordIndex(int x, int y)
{
	return Vector2Int(x - _minx, _maxy - y);
}

SharedObject Map::Find(Vector2Int cell_pos)
{
	if (false == IsWithinBounds(cell_pos.x, cell_pos.y))
		return nullptr;

	Vector2Int index_pos = GetCoordIndex(cell_pos);

	return _objects[index_pos.y][index_pos.x];
}

#pragma region A* path finding
int32 Map::Heuristic(const Vector2Int& start, const Vector2Int& dest)
{
	return abs(dest.x - start.x) + abs(dest.y - start.y);
}

xvector<Vector2Int>& Map::CalculatePath(SharedNode dest_node)
{
	while (dest_node)
	{
		_cell_path.push_back(dest_node->cell_pos);
		SharedNode parent = dest_node->parent;
		dest_node->parent = nullptr;
		dest_node = parent;
	}

	reverse(_cell_path.begin(), _cell_path.end());

	return _cell_path;
}

xvector<Vector2Int>& Map::FindPath(Vector2Int start, Vector2Int dest, bool is_through_objects, int32 max_distance)
{
	// parent
	xhash_map<Vector2Int, Vector2Int> parent;

	// heuristic check
	xhash_map<Vector2Int, int32> h_list;

	// open list
	priority_queue<SharedNode, xvector<SharedNode>, NodePredicate> open_list;

	// close list
	unordered_set<Vector2Int> close_list;

	int32 direction_x[] = {1, 0, -1, 0};
	int32 direction_y[] = {0, -1, 0, 1};

	open_list.push(MakeShared<Node>(start, Heuristic(start, dest)));

	parent[start] = start;

	while (!open_list.empty())
	{
		SharedNode current_node = open_list.top();
		open_list.pop();
		
		if (current_node->cell_pos == dest)
			break;

		close_list.insert(current_node->cell_pos);

		for (int i = 0; i < 4; ++i)
		{
			int32 dir_x = direction_x[i];
			int32 dir_y = direction_y[i];

			Vector2Int& cell_pos = current_node->cell_pos;
			Vector2Int next_pos = Vector2Int(cell_pos.x + dir_x, cell_pos.y + dir_y);

			int32 next_heuristic = Heuristic(next_pos, dest);

			if (next_heuristic > max_distance)
				continue;

			auto iter = h_list.find(next_pos);
			if (iter != h_list.end() && h_list[next_pos] <= next_heuristic)
				continue;

			if (dest != next_pos)
			if (!CanGo(next_pos, is_through_objects))
				continue;

			if (close_list.count(next_pos))
				continue;

			SharedNode next_node = MakeShared<Node>(next_pos, next_heuristic, current_node);

			open_list.push(next_node);
			parent[next_pos] = cell_pos;
			h_list[next_pos] = next_heuristic;
		}
	}

	return CalculatePath(parent, dest);
}
#pragma endregion
