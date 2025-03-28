#pragma once
class Zone
{
public:
	unordered_set<SharedPlayer>& GetPlayers();
	void Remove(SharedObject object);

private:
	unordered_set<SharedPlayer> _players;
};
