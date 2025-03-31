#pragma once
class Zone
{
public:
	Zone(int32 y, int32 x);

	xhash_set<SharedPlayer>& GetPlayers();
	xhash_set<SharedMonster>& GetMonsters();
	xhash_set<SharedProjectile>& GetProjectiles();

	void Remove(SharedObject object);

private:
	xhash_set<SharedPlayer> _players;
	xhash_set<SharedMonster> _monsters;
	xhash_set<SharedProjectile> _projectiles;

	int32 _index_x;
	int32 _index_y;
};
