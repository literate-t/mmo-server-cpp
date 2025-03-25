#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"

void Room::Enter(SharedPlayer player)
{
	//WRITE_LOCK;
	_players[player->PlayerDbId] = player;
}

void Room::Leave(SharedPlayer player)
{
	//WRITE_LOCK;
	_players.erase(player->PlayerDbId);
}

void Room::Broatcast(SharedSendBuffer send_buffer)
{
	//WRITE_LOCK;
	for (auto& p : _players)
	{
		p.second->OwnerSession->Send(send_buffer);
	}
}

void Room::Init(int32 map_id, int32 zone_cell_size)
{
}
