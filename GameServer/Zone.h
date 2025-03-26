#pragma once
class Zone
{
public:
	unordered_set<SharedPlayer>& GetPlayers();

private:
	unordered_set<SharedPlayer> _players;
};
