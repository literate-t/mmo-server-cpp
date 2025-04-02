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

bool Handle_C_Move(SharedPacketSession& session, Protocol::C_Move& pkt)
{
	shared_ptr<GameSession> game_session = static_pointer_cast<GameSession>(session);
	SharedPlayer player = game_session->GetPlayer();
	SharedRoom room = player->GetRoom();

	room->PushJobAsync(&Room::HandleMovePacket, player, pkt);

	return true;
}

bool Handle_C_Skill(SharedPacketSession& session, Protocol::C_Skill& pkt)
{
	return false;
}

bool Handle_C_Login(SharedPacketSession& session, Protocol::C_Login& pkt)
{
	shared_ptr<GameSession> game_session = static_pointer_cast<GameSession>(session);
	game_session->HandleLogin(pkt);

	return true;
}

bool Handle_C_CreatePlayer(SharedPacketSession& session, Protocol::C_CreatePlayer& pkt)
{
	shared_ptr<GameSession> game_session = static_pointer_cast<GameSession>(session);
	game_session->HandleCreatePlayer(pkt);

	return true;
}

bool Handle_C_EnterGame(SharedPacketSession& session, Protocol::C_EnterGame& pkt)
{
	shared_ptr<GameSession> game_session = static_pointer_cast<GameSession>(session);
	game_session->HandleEnterGame(pkt);

	return true;
}

bool Handle_C_EquipItem(SharedPacketSession& session, Protocol::C_EquipItem& pkt)
{
	return false;
}

bool Handle_C_Pong(SharedPacketSession& session, Protocol::C_Pong& pkt)
{
	return false;
}


