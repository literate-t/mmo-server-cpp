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

// 구현부는 작업자가 직접 작성해줘야 한다
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
	// 해킹이 돼서 플레이어가 필드에 있는데도 로그인 패킷이 올 수도 있다
	// 로비에 입장해있는 등의 특정 상태에서만 로그잇 패킷을 처리하겠다는 검증이 필요할 듯

	Protocol::S_LOGIN login_pkt;
	login_pkt.set_success(true);

	// DB에서 플레이어 정보를 가져온다
	// AccoutSession 클래스를 만들어서 정보를 메모리에 저장하는 게 좋다

	// in-game id 발급
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

	// 아직은 read-only라서 락 안 걸어도 된다
	// Handle_C_LOGIN에서 플레이어 정보를 메모리에 저장해놔서 db 접근 안 해도 된다
	//SharedPlayer player = game_session->GetPlayer(index);
	game_session->GetPlayer() = game_session->GetPlayer(index);
	game_session->GetRoom() = g_room;

	g_room->PushJobAsync([game_session]()
		{
			g_room->Enter(game_session->GetPlayer());
		});
	//g_room->PushJob(&Room::Enter, move(player_copy));
	

	// TODO: 패킷 송신 시점도 변경되어야 할 수 있다
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

	// 같은 방에 있는 플레이어에게 브로드캐스팅
	Protocol::S_CHAT chat_pkt;
	chat_pkt.set_msg(pkt.msg());
	auto send_buffer = ClientPacketHandler::MakeSendBuffer(chat_pkt);

	//g_room.PushJob(&Room::Broatcast, send_buffer);
	g_room->PushJobAsync(&Room::Broatcast, move(send_buffer));

	return true;
}
