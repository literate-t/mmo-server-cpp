#pragma once
class Map
{
public:
	void LoadMap(int32 map_id, string prefix_path = "D:/SourceCode/GameSource/MMO_Basic_Cpp/Common/MapData");
	bool ApplyMove(SharedObject object, Vector2Int dest, bool is_through_objects = false, bool apply_collision = true);
	bool CanGo(Vector2Int cell_pos, bool, bool is_through_objects = false);

	Vector2Int GetCoordIndex(Vector2Int cell_pos);
	Vector2Int GetCoordIndex(int x, int y);

	int32 GetMinX() const { return _minx; }
	int32 GetMinY() const { return _miny; }
	int32 GetMaxX() const { return _maxx; }
	int32 GetMaxY() const { return _maxy; }

	int32 GetSizeX() const { return _maxx - _minx + 1; }
	int32 GetSizeY() const { return _maxy - _miny + 1; }

private:
	void InitArrays(int32 row, int32 col);
	bool HasCollision(Vector2Int index_pos, bool is_through_objects);
	bool IsWithinBounds(int32 x, int32 y);
	void SetCurrentZone(SharedObject object, Vector2Int dest);

private:
	int32 _minx;
	int32 _miny;
	int32 _maxx;
	int32 _maxy;

	xvector<xvector<bool>> _collisions;
	xvector<xvector<SharedObject>> _objects;
};

