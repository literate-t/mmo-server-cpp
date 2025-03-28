#include "pch.h"
#include "Zone.h"
#include "Protocol.pb.h"
using namespace Protocol;
#include "ObjectManager.h"

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
        _players.erase(SharedCast<Player>(object));
    }
}
