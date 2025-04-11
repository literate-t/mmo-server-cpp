#include "pch.h"
#include "DBSerializer.h"
#include "Player.h"
#include "Room.h"
#include "Info.h"
#include "Item.h"
#include "GameSession.h"
#include "ClientPacketHandler.h"

void DBSerializer::SavePlayerStatusNoti(SharedPlayer player)
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

void DBSerializer::SavePlayerReward(SharedPlayer player, SharedRoom room, RewardData reward)
{
	if (player == nullptr || room == nullptr)
		return;

	optional<int32> slot = player->GetInventory().GetEmptySlot();
	if (!slot)
	{
		// TODO: ¿ìÆí?
		return;
	}

	int32 slot_value = slot.value();
	// Save hp to db for now
	Instance()->PushJobAsync([player, slot_value, reward, room]() {
		DBConnection* conn = g_db_connection_pool->Pop();
		auto query = L"INSERT INTO items (data_sheet_id, count, slot, equipped, owner_id)	\
			 VALUES (?, ?, ?, ?, ?);";

		DBBind<5, 0> db_bind(*conn, query);

		ItemDB item_db;
		item_db.DataSheetId = reward.itemId;
		item_db.Count = reward.count;
		item_db.Equipped = false;
		item_db.Slot = slot_value;
		item_db.OwnerId = player->PlayerDbId;

		db_bind.BindParam(0, item_db.DataSheetId);
		db_bind.BindParam(1, item_db.Count);
		db_bind.BindParam(2, item_db.Slot);
		db_bind.BindParam(3, item_db.Equipped);
		db_bind.BindParam(4, item_db.OwnerId);

		if (!db_bind.Execute())
			throw new runtime_error("DB Failed: SavePlayerReward()");

		g_db_connection_pool->Push(conn);

		room->PushJobAsync([item_db, player]() {
			SharedItem item = Item::MakeItem(item_db);
			player->GetInventory().Add(item);

			S_AddItem add_item;
			ItemInfo* item_info = add_item.add_items();
			item_info->MergeFrom(item->GetItemInfo());

			player->OwnerSession->Send(ClientPacketHandler::MakeSendBuffer(add_item));
			});
		});
}
