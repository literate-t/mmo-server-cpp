#include "pch.h"
#include "ServerPacketHandler.h"
#include "ServerSession.h"
#include "Util.h"

PacketHandlerFunc g_packet_handler[HANDLER_MAX];

bool ServerPacketHandler::HandlePacket(SharedPacketSession& session, BYTE* buffer, int32 length)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	return g_packet_handler[header->id](session, buffer, length);
}

bool Handle_INVALID(SharedPacketSession& session, BYTE* buffer, int32 length)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : log
	return false;
}

bool Handle_S_EnterGame(SharedPacketSession& session, Protocol::S_EnterGame& pkt)
{
	return false;
}

bool Handle_S_LeaveGame(SharedPacketSession& session, Protocol::S_LeaveGame& pkt)
{
	return false;
}

bool Handle_S_Spawn(SharedPacketSession& session, Protocol::S_Spawn& pkt)
{
	return false;
}

bool Handle_S_Despawn(SharedPacketSession& session, Protocol::S_Despawn& pkt)
{
	return false;
}

bool Handle_S_Move(SharedPacketSession& session, Protocol::S_Move& pkt)
{
	return false;
}

bool Handle_S_Skill(SharedPacketSession& session, Protocol::S_Skill& pkt)
{
	return false;
}

bool Handle_S_ChangeHp(SharedPacketSession& session, Protocol::S_ChangeHp& pkt)
{
	return false;
}

bool Handle_S_Die(SharedPacketSession& session, Protocol::S_Die& pkt)
{
	return false;
}

bool Handle_S_Connected(SharedPacketSession& session, Protocol::S_Connected& pkt)
{
	SharedServerSession server_session = static_pointer_cast<ServerSession>(session);

	C_Login login_packet;
	login_packet.set_uniqueid("Dummy_" + Util::ToString(server_session->GetId(), 3));

	server_session->Send(ServerPacketHandler::MakeSendBuffer(login_packet));

	return true;
}

bool Handle_S_Login(SharedPacketSession& session, Protocol::S_Login& pkt)
{
	SharedServerSession server_session = static_pointer_cast<ServerSession>(session);

	if (pkt.players_size() == 0)
	{
		C_CreatePlayer create_packet;
		create_packet.set_name("Dummy_" + Util::ToString(server_session->GetId()));
		server_session->Send(ServerPacketHandler::MakeSendBuffer(create_packet));
	}
	else
	{
		auto& players = pkt.players();
		auto& player = players.Get(0);

		C_EnterGame enter_game;
		enter_game.set_name(player.name());

		server_session->Send(ServerPacketHandler::MakeSendBuffer(enter_game));
	}

	return true;
}

bool Handle_S_CreatePlayer(SharedPacketSession& session, Protocol::S_CreatePlayer& pkt)
{
	SharedServerSession server_session = static_pointer_cast<ServerSession>(session);

	C_EnterGame enter_packet;
	enter_packet.set_name(pkt.player().name());

	server_session->Send(ServerPacketHandler::MakeSendBuffer(enter_packet));

	return true;
}

bool Handle_S_ItemList(SharedPacketSession& session, Protocol::S_ItemList& pkt)
{
	return false;
}

bool Handle_S_AddItem(SharedPacketSession& session, Protocol::S_AddItem& pkt)
{
	return false;
}

bool Handle_S_EquipItem(SharedPacketSession& session, Protocol::S_EquipItem& pkt)
{
	return false;
}

bool Handle_S_UseItem(SharedPacketSession& session, Protocol::S_UseItem& pkt)
{
	return false;
}

bool Handle_S_ChangeStat(SharedPacketSession& session, Protocol::S_ChangeStat& pkt)
{
	return false;
}

bool Handle_S_Ping(SharedPacketSession& session, Protocol::S_Ping& pkt)
{
	C_Pong pong;
	session->Send(ServerPacketHandler::MakeSendBuffer(pong));

	return true;
}
