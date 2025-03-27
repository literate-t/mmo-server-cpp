#include "pch.h"
#include "Map.h"
#include "GameObject.h"
#include "StringReader.h"
#include "Util.h"
void Map::InitArrays(int32 row, int32 col)
{
	// _collisions
	_collisions.resize(col);
	for (int i = 0; i < _collisions.size(); ++i)
	{
		_collisions[i].resize(row);
		for (int j = 0; j < _collisions[i].size(); ++j)
			_collisions[i][j] = false;
	}

	// _objects
	_objects.resize(col);
	for (int i = 0; i < _objects.size(); ++i)
	{
		_objects[i].resize(row);
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
	InitArrays(size_y, size_x);

	for (int32 y = 0; y < size_y; ++y)
	{
		string line = reader.ReadLine();
		for (int32 x = 0; x < size_x; ++x)
			_collisions[y][x] = line[x] == '1' ? true : false;
	}
}

bool Map::ApplyMove(SharedObject object, Vector2Int dest, bool is_through_objects, bool apply_collision)
{
bool Map::CanGo(Vector2Int cell_pos, bool, bool is_through_objects)
{
	if (cell_pos.x < _minx || cell_pos.y > _maxx) return false;
	if (cell_pos.y < _miny || cell_pos.y > _maxy) return false;

	Vector2Int index_pos = GetCoordIndex(cell_pos);
	
	return !HasCollision(index_pos, is_through_objects);
}

bool Map::HasCollision(Vector2Int index_pos, bool is_through_objects)
{	
	return _collisions[index_pos.y][index_pos.x] || (!is_through_objects && _objects[index_pos.y][index_pos.x] != nullptr);
}
	return true;
}
