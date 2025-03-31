#pragma once
class Zone
{
public:
	Zone(int32 y, int32 x);

	unordered_set<SharedPlayer>& GetPlayers();
	unordered_set<SharedMonster>& GetMonsters();
	unordered_set<SharedProjectile>& GetProjectiles();

	void Remove(SharedObject object);

private:
	unordered_set<SharedPlayer> _players;
	unordered_set<SharedMonster> _monsters;
	unordered_set<SharedProjectile> _projectiles;

	int32 _index_x;
	int32 _index_y;
};
