#include "pch.h"
#include "ClientPacketHandler.h"
#include "GameSession.h"
#include "Player.h"
#include "Room.h"

PacketHandlerFunc g_packet_handler[HANDLER_MAX];

bool ClientPacketHandler::HandlePacket(SharedPacketSession& session, BYTE* buffer, int32 length)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	return g_packet_handler[header->id](session, buffer, length);
}

// �����δ� �۾��ڰ� ���� �ۼ������ �Ѵ�
bool Handle_INVALID(SharedPacketSession& session, BYTE* buffer, int32 length)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : log
	return false;
}

bool Handle_C_LOGIN(SharedPacketSession& session, Protocol::C_LOGIN& pkt)
{
	SharedGameSession game_session = static_pointer_cast<GameSession>(session);

	// TODO : validation
	// ��ŷ�� �ż� �÷��̾ �ʵ忡 �ִµ��� �α��� ��Ŷ�� �� ���� �ִ�
	// �κ� �������ִ� ���� Ư�� ���¿����� �α��� ��Ŷ�� ó���ϰڴٴ� ������ �ʿ��� ��

	Protocol::S_LOGIN login_pkt;
	login_pkt.set_success(true);

	// DB���� �÷��̾� ������ �����´�
	// AccoutSession Ŭ������ ���� ������ �޸𸮿� �����ϴ� �� ����

	// in-game id �߱�
	static Atomic<uint64> id_generator = 1;

	// #player1
	{
		auto player = login_pkt.add_player();
		player->set_name(u8"name_from_db_1");
		player->set_player_type(Protocol::PLAYER_TYPE_KNIGHT);

		SharedPlayer player_ptr = MakeShared<Player>();
		player_ptr->player_id = id_generator++;
		player_ptr->name = player->name();
		player_ptr->type = player->player_type();
		player_ptr->owner_session = game_session;
		game_session->AddPlayer(player_ptr);
	}

	// #player1
	{
		auto player = login_pkt.add_player();
		player->set_name(u8"name_from_db_2");
		player->set_player_type(Protocol::PLAYER_TYPE_MAGE);

		SharedPlayer player_ptr = MakeShared<Player>();
		player_ptr->player_id = id_generator++;
		player_ptr->name = player->name();
		player_ptr->type = player->player_type();
		player_ptr->owner_session = game_session;
		game_session->AddPlayer(player_ptr);
	}

	auto send_buffer = ClientPacketHandler::MakeSendBuffer(login_pkt);
	session->Send(send_buffer);

	return true;
}

bool Handle_C_ENTER_GAME(SharedPacketSession& session, Protocol::C_ENTER_GAME& pkt)
{
	SharedGameSession game_session = static_pointer_cast<GameSession>(session);

	uint64 index = pkt.player_index();

	// TODO : validate index

	// ������ read-only�� �� �� �ɾ �ȴ�
	// Handle_C_LOGIN���� �÷��̾� ������ �޸𸮿� �����س��� db ���� �� �ص� �ȴ�
	//SharedPlayer player = game_session->GetPlayer(index);
	game_session->GetPlayer() = game_session->GetPlayer(index);
	game_session->GetRoom() = g_room;

	g_room->PushJobAsync([game_session]()
		{
			g_room->Enter(game_session->GetPlayer());
		});
	//g_room->PushJob(&Room::Enter, move(player_copy));
	

	// TODO: ��Ŷ �۽� ������ ����Ǿ�� �� �� �ִ�
	Protocol::S_ENTER_GAME enter_game_pkt;
	enter_game_pkt.set_success(true);
	auto send_buffer = ClientPacketHandler::MakeSendBuffer(enter_game_pkt);

	//session->Send(send_buffer);	
	game_session->Send(send_buffer);

	return true;
}

bool Handle_C_CHAT(SharedPacketSession& session, Protocol::C_CHAT& pkt)
{
	printf("server: %s\n", pkt.msg().c_str());

	// ���� �濡 �ִ� �÷��̾�� ��ε�ĳ����
	Protocol::S_CHAT chat_pkt;
	chat_pkt.set_msg(pkt.msg());
	auto send_buffer = ClientPacketHandler::MakeSendBuffer(chat_pkt);

	//g_room.PushJob(&Room::Broatcast, send_buffer);
	g_room->PushJobAsync(&Room::Broatcast, move(send_buffer));

	return true;
}
