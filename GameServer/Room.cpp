#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"

shared_ptr<Room> g_room = make_shared<Room>();

void Room::Enter(SharedPlayer player)
{
	//WRITE_LOCK;
	_players[player->player_id] = player;
}

void Room::Leave(SharedPlayer player)
{
	//WRITE_LOCK;
	_players.erase(player->player_id);
}

void Room::Broatcast(SharedSendBuffer send_buffer)
{
	//WRITE_LOCK;
	for (auto& p : _players)
	{
		p.second->owner_session->Send(send_buffer);
	}
}