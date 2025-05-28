#include "pch.h"
#include "ClientPacketHandler.h"
#include "GameSession.h"
#include "Player.h"
#include "Monster.h"
#include "Room.h"
#include "Heartbeat.h"
#include "ObjectManager.h"

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

bool Handle_C_Move(SharedPacketSession& session, const Protocol::C_Move& pkt)
{
	shared_ptr<GameSession> game_session = static_pointer_cast<GameSession>(session);
	SharedPlayer player = game_session->GetPlayer();
	SharedRoom room = player->GetRoom();

	room->PushJobAsync(&Room::HandleMovePacket, player, pkt);

	return true;
}

bool Handle_C_Skill(SharedPacketSession& session, const Protocol::C_Skill& pkt)
{
	shared_ptr<GameSession> game_session = static_pointer_cast<GameSession>(session);
	SharedPlayer player = game_session->GetPlayer();
	SharedRoom room = player->GetRoom();

	room->PushJobAsync(&Room::HandleSkillPacket, player, pkt);

	return true;
}

bool Handle_C_Login(SharedPacketSession& session, const Protocol::C_Login& pkt)
{
	shared_ptr<GameSession> game_session = static_pointer_cast<GameSession>(session);
	game_session->HandleLogin(pkt);

	return true;
}

bool Handle_C_CreatePlayer(SharedPacketSession& session, const Protocol::C_CreatePlayer& pkt)
{
	shared_ptr<GameSession> game_session = static_pointer_cast<GameSession>(session);
	game_session->HandleCreatePlayer(pkt);

	return true;
}

bool Handle_C_EnterGame(SharedPacketSession& session, const Protocol::C_EnterGame& pkt)
{
	shared_ptr<GameSession> game_session = static_pointer_cast<GameSession>(session);
	game_session->HandleEnterGame(pkt);

	return true;
}

bool Handle_C_EquipItem(SharedPacketSession& session, const Protocol::C_EquipItem& pkt)
{
	shared_ptr<GameSession> game_session = static_pointer_cast<GameSession>(session);
	SharedPlayer player = game_session->GetPlayer();
	SharedRoom room = player->GetRoom();

	room->PushJobAsync(&Room::HandleEquipItemPacket, player, pkt);
	return true;
}

bool Handle_C_UseItem(SharedPacketSession& session, const Protocol::C_UseItem& pkt)
{
	shared_ptr<GameSession> game_session = static_pointer_cast<GameSession>(session);
	SharedPlayer player = game_session->GetPlayer();
	SharedRoom room = player->GetRoom();

	room->PushJobAsync(&Room::HandleUseItemPacket, player, pkt);
	return false;
}

bool Handle_C_DropItem(SharedPacketSession& session, const Protocol::C_DropItem& pkt)
{
	shared_ptr<GameSession> game_session = static_pointer_cast<GameSession>(session);
	SharedPlayer player = game_session->GetPlayer();
	SharedRoom room = player->GetRoom();

	room->PushJobAsync(&Room::HandleDropItemPacket, player, pkt);

	return false;
}

bool Handle_C_Pong(SharedPacketSession& session, const Protocol::C_Pong& pkt)
{	
	g_shared_heart->HandlePong(static_pointer_cast<GameSession>(session));
	return true;
}

bool Handle_C_AnimEnd(SharedPacketSession& session, const Protocol::C_AnimEnd& pkt)
{
	shared_ptr<GameSession> game_session = static_pointer_cast<GameSession>(session);
	SharedPlayer player = game_session->GetPlayer();
	
	SharedRoom room = player->GetRoom();
	GameObjectType type = g_object_manager->GetObjectTypeById(pkt.objectid());

	SharedObject object;
	if (type == GameObjectType::PLAYER)
		object = room->GetPlayer(pkt.objectid());
	else if (type == GameObjectType::MONSTER)
		object = room->GetMonster(pkt.objectid());

	object->OnDeadAnim();

	return true;
}

bool Handle_C_GoRandom(SharedPacketSession& session, const Protocol::C_GoRandom& pkt)
{
	shared_ptr<GameSession> game_session = static_pointer_cast<GameSession>(session);
	SharedPlayer player = game_session->GetPlayer();
	SharedRoom room = player->GetRoom();

	room->PushJobAsync(&Room::HandleGoRandomPacket, player, pkt);

	return true;
}


