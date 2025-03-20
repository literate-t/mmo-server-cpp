#include "pch.h"
#include "DBSerializer.h"
#include "Player.h"
#include "Room.h"

void DBSerializer::SavePlayerStatusAll(SharedPlayer player, SharedRoom room)
{
	if (player == nullptr || room == nullptr)
		return;

	// TODO : dbø° ¿˙¿Â
	Instance().PushJobAsync([&player, &room](){

	});
}
