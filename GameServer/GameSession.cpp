#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ClientPacketHandler.h"
#include "Room.h"
#include "Info.h"

GameSession::~GameSession()
{
	puts("~GameSession");
}

void GameSession::OnConnectCompleted()
{
	g_session_manager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnectCompleted()
{
	g_session_manager.Remove(static_pointer_cast<GameSession>(shared_from_this()));

	// player와의 순환을 끊어준다
	if (_current_player)
	{
		if (auto room = _room.lock())
		{
			room->PushJobAsync(&Room::Leave, move(_current_player));
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

	_lobby_players.clear();

	DBConnection* conn = g_db_connection_pool->Pop();
	auto query = L"														\
		SELECT account_id, player_id, player_name,						\
		level, hp, max_hp, attack, speed, total_exp						\
		FROM accounts													\
		LEFT JOIN players ON accounts.account_id = players.account_id	\
		WHERE account_name = (?)";

	int32 account_id = 0;
	int32 player_id = 0;
	string player_name;
	player_name.resize(128);
	Stat stat;

	DBBind<1, 3> db_bind(*conn, query);

	db_bind.BindParam(0, login_packet.uniqueid().c_str());
	db_bind.BindColumn(0, account_id);
	db_bind.BindColumn(1, player_id);
	db_bind.BindColumn(2, &player_name[0], player_name.size() * sizeof(wchar_t));
	db_bind.BindColumn(3, stat.level);
	db_bind.BindColumn(4, stat.hp);
	db_bind.BindColumn(5, stat.max_hp);
	db_bind.BindColumn(6, stat.attack);
	db_bind.BindColumn(7, stat.speed);
	db_bind.BindColumn(8, stat.total_exp);

	db_bind.Execute();

	int is_fetch_data = false;
	S_Login login_ok;
	while (db_bind.Fetch())
	{
		is_fetch_data = true;

		LobbyPlayerInfo* lobby_player_info = login_ok.add_players();

		lobby_player_info->set_playerdbid(player_id);
		lobby_player_info->set_name(player_name.c_str());

		StatInfo* stat_info = new StatInfo();
		stat_info->set_level(stat.level);
		stat_info->set_hp(stat.hp);
		stat_info->set_maxhp(stat.max_hp);
		stat_info->set_attack(stat.attack);
		stat_info->set_speed(stat.speed);
		stat_info->set_totalexp(stat.total_exp);

		lobby_player_info->set_allocated_statinfo(stat_info);

		_lobby_players.push_back(lobby_player_info);
		
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
		db_bind.Execute();

		int64 newId;
		db_bind.GetId(newId);
		_account_id = newId;

		S_Login login_ok;
		login_ok.set_loginok(1);

		Send(ClientPacketHandler::MakeSendBuffer(login_ok));
	}

	_server_state = PlayerServerState::SERVER_STATE_LOBBY;

	g_db_connection_pool->Push(conn);
}
