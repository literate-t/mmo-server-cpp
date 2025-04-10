#pragma once

using SharedGameSession = shared_ptr<class GameSession>;
using SharedObject = shared_ptr<class GameObject>;
using SharedPlayer = shared_ptr<class Player>;
using SharedMonster = shared_ptr<class Monster>;
using SharedProjectile = shared_ptr<class Projectile>;
using SharedArrow = shared_ptr<class Arrow>;
using SharedRoom = shared_ptr<class Room>;
using SharedItem = shared_ptr<class Item>;
using SharedWeapon = shared_ptr<class Weapon>;
using SharedArmor = shared_ptr<class Armor>;
using SharedInventory = shared_ptr<class Inventory>;
using SharedViewCube = shared_ptr<class ViewCube>;
using SharedMap = shared_ptr<class Map>;
using SharedZone = shared_ptr<class Zone>;
using SharedDBSerializer = shared_ptr<class DBSerializer>;

struct Vector2Int
{
	int x;
	int y;

	Vector2Int() :x(-1), y(-1) {}
	Vector2Int(int x, int y) : x(x), y(y) {}

	static Vector2Int Up() { return Vector2Int(0, 1); }
	static Vector2Int Down() { return Vector2Int(0, -1); }
	static Vector2Int Left() { return Vector2Int(-1, 0); }
	static Vector2Int Right() { return Vector2Int(1, 0); }

	Vector2Int& operator=(const Vector2Int& rhs)
	{
		x = rhs.x;
		y = rhs.y;

		return *this;
	}

	Vector2Int operator+(const Vector2Int& rhs) const
	{
		return Vector2Int(x + rhs.x, y + rhs.y);
	}

	Vector2Int& operator+=(const Vector2Int& rhs)
	{
		x += rhs.x;
		y += rhs.y;

		return *this;
	}

	Vector2Int operator-(const Vector2Int& rhs) const
	{
		return Vector2Int(x - rhs.x, y - rhs.y);
	}

	bool operator==(const Vector2Int& rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}

	bool operator!=(const Vector2Int& rhs) const
	{
		return !operator==(rhs);
	}

	const int32 SquareMagnitude = x * x + y * y;
	float Magnitude() { return static_cast<float>(sqrt(SquareMagnitude)); }

	const int32 SimpleDistance = abs(x) + abs(y);
};