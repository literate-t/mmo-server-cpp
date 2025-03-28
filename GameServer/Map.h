#pragma once

using SharedNode = shared_ptr<struct Node>;
class Map
{
public:
	void LoadMap(int32 map_id, string prefix_path = "D:/SourceCode/GameSource/MMO_Basic_Cpp/Common/MapData");

	bool ApplyMove(SharedObject object, Vector2Int dest, bool is_through_objects = false, bool apply_collision = true);
	bool ApplyLeave(SharedObject object);

	bool CanGo(Vector2Int cell_pos, bool is_through_objects = false);

	Vector2Int GetCoordIndex(Vector2Int cell_pos);
	Vector2Int GetCoordIndex(int x, int y);

	int32 GetMinX() const { return _minx; }
	int32 GetMinY() const { return _miny; }
	int32 GetMaxX() const { return _maxx; }
	int32 GetMaxY() const { return _maxy; }

	int32 GetSizeX() const { return _maxx - _minx + 1; }
	int32 GetSizeY() const { return _maxy - _miny + 1; }

	xvector<Vector2Int>& FindPath(Vector2Int start, Vector2Int dest, bool is_through_objects = true, int32 max_distance = 10);

private:
	void InitArrays(int32 row, int32 col);
	bool HasCollision(Vector2Int index_pos, bool is_through_objects);
	bool IsWithinBounds(int32 x, int32 y);
	void SetCurrentZone(SharedObject object, Vector2Int dest);
	int32 Heuristic(const Vector2Int& start, const Vector2Int& dest);

	xvector<Vector2Int>& CalculatePath(SharedNode dest_node);

private:
	int32 _minx;
	int32 _miny;
	int32 _maxx;
	int32 _maxy;

	xvector<xvector<bool>> _collisions;
	xvector<xvector<SharedObject>> _objects;
	xvector<Vector2Int> _cell_path;
};

struct Node
{
	Node(Vector2Int pos, int32 g, int32 h, SharedNode parent = nullptr)
		: cell_pos(pos), g(g), h(h), parent(parent)
	{
	}

	Vector2Int cell_pos;
	int32 g;
	int32 h;
	SharedNode parent;

	int32 calc() const { return g + h; }
};

struct NodePredicate
{
	bool operator()(const SharedNode& a, const SharedNode& b)
	{
		return a->calc() > b->calc();
	}
};

namespace std
{
	template<>
	struct hash<Vector2Int>
	{
		size_t operator()(const Vector2Int& item) const
		{
			size_t h1 = hash<int32>()(item.x);
			size_t h2 = hash<int32>()(item.y);

			return h1 ^ (h2 << 1);
		}
	};
}