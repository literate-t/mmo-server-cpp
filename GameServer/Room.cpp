#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"
#include "ObjectManager.h"
#include "Map.h"
#include "Zone.h"
#include "ClientPacketHandler.h"
#include "ViewCube.h"

void Room::Enter(SharedObject object)
{
	if (object == nullptr)
		return;

	// TODO: random position

	GameObjectType type = g_object_manager->GetObjectTypeById(object->GetObjectId());

	if (type == GameObjectType::PLAYER)
	{
		SharedPlayer player = static_pointer_cast<Player>(object);
		_players[player->PlayerDbId] = player;
		
		player->SetRoom(static_pointer_cast<Room>(shared_from_this()));
		player->RefreshStat();

		g_map->ApplyMove(player, Vector2Int(player->GetPositionInfo().posx(), player->GetPositionInfo().posy()));
		GetZone(player->GetCellPos()).GetPlayers().insert(player);

		// 접속 본인 정보 전송
		{
			S_EnterGame enter_game;
			enter_game.set_allocated_player(&player->GetObjectInfo());
			player->OwnerSession->Send(ClientPacketHandler::MakeSendBuffer(enter_game));
		}

		player->GetView()->IsReset = true;
		player->GetView()->Update();
	}
}

void Room::Leave(SharedObject player)
{
	//_players.erase(player->PlayerDbId);
}

void Room::Broatcast(SharedSendBuffer send_buffer)
{
	//WRITE_LOCK;
	for (auto& p : _players)
	{
		p.second->OwnerSession->Send(send_buffer);
	}
}

Zone Room::GetZone(Vector2Int cell_pos)
{
	return Zone();
}

void Room::Init(int32 map_id, int32 zone_cell_size)
{
}
