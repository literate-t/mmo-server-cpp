#include "pch.h"
#include "Zone.h"

unordered_set<SharedPlayer>& Zone::GetPlayers()
{
    return _players;
}
