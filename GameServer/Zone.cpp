#include "pch.h"
#include "Zone.h"
#include "Protocol.pb.h"
using namespace Protocol;
#include "ObjectManager.h"
#include "Player.h"

Zone::Zone(int32 y, int32 x)
    :_index_y(y), _index_x(x)
{
}

unordered_set<SharedPlayer>& Zone::GetPlayers()
{
    return _players;
}

void Zone::Remove(SharedObject object)
{
    GameObjectType type = g_object_manager->GetObjectTypeById(object->GetObjectId());

    switch (type)
    {
    case GameObjectType::PLAYER:
        _players.erase(static_pointer_cast<Player>(object));
    }
}
