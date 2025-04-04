#include "pch.h"
#include "DBSerializer.h"
#include "Player.h"
#include "Room.h"

void DBSerializer::SavePlayerStatusAll(SharedPlayer player)
{
	if (player == nullptr)
		return;

	// Save hp to db for now
	Instance()->PushJobAsync([player]() {
		DBConnection* conn = g_db_connection_pool->Pop();
		auto query = L"UPDATE stats SET hp = (?) WHERE player_id = (?)";

		DBBind<2, 0> db_bind(*conn, query);
		int32 hp = player->GetStatInfo().hp();

		db_bind.BindParam(0, hp);
		db_bind.BindParam(1, player->PlayerDbId);

		if (!db_bind.Execute())
			throw new runtime_error("DB Failed: SavePlayerStatusAll()");

		g_db_connection_pool->Push(conn);
	});
}