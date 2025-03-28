#pragma once
class Zone
{
public:
	Zone(int32 y, int32 x);

	unordered_set<SharedPlayer>& GetPlayers();
	void Remove(SharedObject object);

private:
	unordered_set<SharedPlayer> _players;

	int32 _index_x;
	int32 _index_y;
};
