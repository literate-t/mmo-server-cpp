#include "pch.h"
#include "Map.h"
#include "GameObject.h"
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

bool Map::ApplyMove(SharedObject object, Vector2Int dest, bool is_through_objects, bool apply_collision)
{
	return true;
}
