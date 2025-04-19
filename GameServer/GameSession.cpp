#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ClientPacketHandler.h"
#include "DataManager.h"
#include "Room.h"
#include "Info.h"
#include "ObjectManager.h"
#include "Player.h"
#include "DataModel.h"
using namespace DataModel;
#include "Item.h"
#include "Inventory.h"
#include "RoomManager.h"
#include "Heartbeat.h"

GameSession::~GameSession()
{
	puts("~GameSession");
}

void GameSession::OnConnectCompleted()
{
	g_session_manager.Add(static_pointer_cast<GameSession>(shared_from_this()));

	S_Connected connected_packet;
	Send(ClientPacketHandler::MakeSendBuffer(connected_packet));

	HeartbeatTick = GetTickCount64();
	g_shared_heart->PushTimerAsync(5000, &Heartbeat::Ping, static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnectCompleted()
{
	g_session_manager.Remove(static_pointer_cast<GameSession>(shared_from_this()));

	// player와의 순환을 끊어준다
	if (_current_player)
	{
		if (auto room = _room.lock())
		{
			room->PushJobAsync(&Room::Leave, _current_player->GetObjectId());
		}
	}

	_current_player = nullptr;
	_players.clear();
}

void GameSession::OnRecvPacketProcessed(BYTE* buffer, int32 length)
{
	SharedPacketSession session = GetSharedPacketSession();
	ClientPacketHandler::HandlePacket(session, buffer, length);
}

void GameSession::Send(SharedSendBuffer send_buffer)
{
	bool sendible = false;
	{
		WRITE_LOCK;
		_send_packets.push(send_buffer);
		_pending_bytes += send_buffer->WriteSize();

		if (_pending_bytes >= SEND_BYTE)
			sendible = true;
	}

	if (sendible)
		FlushSend();
	
	bool expected = true;
	bool desired = false;
	if (_can_flush.compare_exchange_strong(expected, desired))
	{
		g_shared_packet_manager->PushTimerAsync(SEND_TICK, [shared = shared_from_this()]()
			{
				SharedGameSession session = static_pointer_cast<GameSession>(shared);
				session->FlushSend();
			});
	}
}

void GameSession::FlushSend()
{
	xqueue<SharedSendBuffer> reserved_packet;

	{
		WRITE_LOCK;
		if (_send_packets.empty())
		{
			_can_flush.store(true);
			return;
		}

		while (!_send_packets.empty())
		{
			reserved_packet.push(_send_packets.front());
			_send_packets.pop();
		}

		_pending_bytes = 0;
	}

	_can_flush.store(true);
	Session::Send(reserved_packet);
}

bool GameSession::CanFlush()
{
	READ_LOCK;
	return !_send_packets.empty();
}

void GameSession::OnSendCompleted(int32 length)
{
	
}

void GameSession::AddPlayer(SharedPlayer player)
{
	_players.push_back(player);
}

void GameSession::RemovePlayer(SharedPlayer player)
{
	auto find_iter = find(_players.begin(), _players.end(), player);
	_players.erase(find_iter);
}

void GameSession::HandleLogin(C_Login& login_packet)
{
	if (_server_state != PlayerServerState::SERVER_STATE_LOGIN)
		return;

	ClearLobbyPlayer();	

	DBConnection* conn = g_db_connection_pool->Pop();
	auto query = L"																		\
		SELECT a.account_id as account_id, p.player_id as player_id, p.player_name as player_name,									\
		s.level as level, s.hp as hp, s.max_hp as max_hp, s.attack, s.speed as speed, s.total_exp as total_exp	\
		FROM accounts a																	\
		LEFT JOIN players as p ON a.account_id = p.account_id				\
		LEFT JOIN stats as s ON p.player_id = s.player_id						\
		WHERE a.account_name = (?)";

	int32 account_id = 0;
	int32 player_id = 0;
	string player_name;
	player_name.resize(128);
	StatData stat;

	DBBind<1, 9> db_bind(*conn, query);

	db_bind.BindParam(0, login_packet.uniqueid().c_str());
	db_bind.BindColumn(0, account_id);
	db_bind.BindColumn(1, player_id);
	db_bind.BindColumn(2, &player_name[0], (int32)player_name.size());
	db_bind.BindColumn(3, stat.level);
	db_bind.BindColumn(4, stat.hp);
	db_bind.BindColumn(5, stat.max_hp);
	db_bind.BindColumn(6, stat.attack);
	db_bind.BindColumn(7, stat.speed);
	db_bind.BindColumn(8, stat.total_exp);

	ASSERT_CRASH(db_bind.Execute());

	bool is_fetch_data = false;
	S_Login login_ok;
	while (db_bind.Fetch())
	{
		is_fetch_data = true;

		LobbyPlayerInfo* lobby_player_info = login_ok.add_players();

		lobby_player_info->set_playerdbid(player_id);
		lobby_player_info->set_name(player_name.c_str());

		StatInfo* stat_info = lobby_player_info->mutable_statinfo();//new StatInfo();
		stat_info->set_level(stat.level);
		stat_info->set_hp(stat.hp);
		stat_info->set_maxhp(stat.max_hp);
		stat_info->set_attack(stat.attack);
		stat_info->set_speed(stat.speed);
		stat_info->set_totalexp(stat.total_exp);

		LobbyPlayerInfo* info = new LobbyPlayerInfo();
		info->MergeFrom(*lobby_player_info);
		_lobby_players.push_back(info);
	}
	if (is_fetch_data == true)
	{
		_account_id = account_id;
		login_ok.set_loginok(1);
		Send(ClientPacketHandler::MakeSendBuffer(login_ok));
	}
	else if (is_fetch_data == false)
	{	
		string account_name = login_packet.uniqueid();
		auto query = L"											\
				INSERT INTO accounts (account_name) VALUES (?);	\
				SELECT SCOPE_IDENTITY() AS NewID;";

		DBBind<1, 0> db_bind(*conn, query);
		db_bind.BindParam(0, account_name.c_str());

		ASSERT_CRASH(db_bind.Execute());

		int32 newId;
		db_bind.GetId(newId);
		_account_id = newId;

		S_Login login_ok;
		login_ok.set_loginok(1);

		Send(ClientPacketHandler::MakeSendBuffer(login_ok));
	}

	_server_state = PlayerServerState::SERVER_STATE_LOBBY;

	g_db_connection_pool->Push(conn);
}

void GameSession::HandleCreatePlayer(C_CreatePlayer packet)
{
	if (_server_state != PlayerServerState::SERVER_STATE_LOBBY)
		return;

	// level 1 stat	
	StatData* stat_data = g_data_manager->Stat(1);

	DBConnection* conn = g_db_connection_pool->Pop();
	auto query = L"INSERT INTO players(player_name, account_id) VALUES (?, ?);\
				   SELECT SCOPE_IDENTITY() as NewId;";

	DBBind<2, 0> db_bind_select(*conn, query);
	db_bind_select.BindParam(0, packet.name().c_str());
	db_bind_select.BindParam(1, _account_id);

	ASSERT_CRASH(db_bind_select.Execute());

	int32 new_player_id;
	db_bind_select.GetId(new_player_id);

	query = L"INSERT INTO stats(level, hp, max_hp, attack, speed, total_exp, player_id) VALUES (?, ?, ?, ?, ?, ?, ?);";
	DBBind<7, 0> db_bind_insert(*conn, query);
	db_bind_insert.BindParam(0, stat_data->level);
	db_bind_insert.BindParam(1, stat_data->hp);
	db_bind_insert.BindParam(2, stat_data->max_hp);
	db_bind_insert.BindParam(3, stat_data->attack);
	db_bind_insert.BindParam(4, stat_data->speed);
	db_bind_insert.BindParam(5, stat_data->total_exp);
	db_bind_insert.BindParam(6, new_player_id);

	ASSERT_CRASH(db_bind_insert.Execute());

	g_db_connection_pool->Push(conn);

	LobbyPlayerInfo* lobby_player_info = new LobbyPlayerInfo();
	lobby_player_info->set_playerdbid(new_player_id);
	lobby_player_info->set_name(packet.name());

	StatInfo* stat_info = lobby_player_info->mutable_statinfo();
	stat_info->set_level(stat_data->level);
	stat_info->set_hp(stat_data->hp);
	stat_info->set_maxhp(stat_data->max_hp);
	stat_info->set_attack(stat_data->attack);
	stat_info->set_speed(stat_data->speed);
	stat_info->set_totalexp(stat_data->total_exp);

	_lobby_players.push_back(lobby_player_info);

	// to client
	S_CreatePlayer new_player_pkt;
	auto new_player = new_player_pkt.mutable_player();
	new_player->MergeFrom(*lobby_player_info);

	Send(ClientPacketHandler::MakeSendBuffer(new_player_pkt));
}

void GameSession::HandleEnterGame(C_EnterGame packet)
{
	if (_server_state != PlayerServerState::SERVER_STATE_LOBBY)
		return;

	string find_name = packet.name();
	auto iter = std::find_if(_lobby_players.begin(), _lobby_players.end(), [&find_name](LobbyPlayerInfo* item) {
		return item->name() == find_name;
		});

	if (iter == _lobby_players.end())
		return;

	_current_player = g_object_manager->Add<Player>();
	{
		LobbyPlayerInfo* find_player = *iter;
		_current_player->PlayerDbId = find_player->playerdbid();
		_current_player->GetObjectInfo().set_name(find_player->name());

		PositionInfo& position_info = _current_player->GetPositionInfo();
		position_info.set_state(EntityState::IDLE);
		position_info.set_movedir(MoveDir::DOWN);
		position_info.set_posx(0);
		position_info.set_posy(0);

		_current_player->GetStatInfo().MergeFrom(find_player->statinfo());
		_current_player->OwnerSession = static_pointer_cast<GameSession>(shared_from_this());

		S_ItemList item_list_packet;
		// Get items
		DBConnection* conn = g_db_connection_pool->Pop();
		auto query = L"SELECT item_id, data_sheet_id, count, slot,\
						equipped, owner_id FROM items WHERE owner_id = (?)";

		DBBind<1, 6> db_bind_select(*conn, query);
		ItemDB item_db;
		int32 player_id = find_player->playerdbid();
		db_bind_select.BindParam(0, player_id);
		db_bind_select.BindColumn(0, item_db.ItemDbId);
		db_bind_select.BindColumn(1, item_db.DataSheetId);
		db_bind_select.BindColumn(2, item_db.Count);
		db_bind_select.BindColumn(3, item_db.Slot);
		db_bind_select.BindColumn(4, item_db.Equipped);
		db_bind_select.BindColumn(5, item_db.OwnerId);

		ASSERT_CRASH(db_bind_select.Execute());
		bool is_fetch = false;

		S_ItemList item_list;
		while (db_bind_select.Fetch())
		{
			is_fetch = true;

			shared_ptr<Item> item = Item::MakeItem(item_db);
			if (item == nullptr)
				continue;

			_current_player->GetInventory().Add(item);
			Protocol::ItemInfo* add_item = item_list.add_items();
			add_item->MergeFrom(item->GetItemInfo());
		}

		Send(ClientPacketHandler::MakeSendBuffer(item_list));

		g_db_connection_pool->Push(conn);
	}

	_server_state = PlayerServerState::SERVER_STATE_GAME;

	_room = g_room_manager->Find(1);
	auto room = _room.lock();
	room->PushJobAsync(&Room::Enter, _current_player, true);

	ClearLobbyPlayer();
}

void GameSession::ClearLobbyPlayer()
{
	for (auto player : _lobby_players)
	{
		StatInfo* stat_info = player->release_statinfo();
		if (stat_info != nullptr)
			delete stat_info;
		
		delete player;
		player = nullptr;
	}
}