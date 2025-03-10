#pragma once

#include "Protocol.pb.h"

constexpr int32 HANDLER_MAX = 0x2000;
using PacketHandlerFunc = std::function<bool(SharedPacketSession&, BYTE*, int32)>;
extern PacketHandlerFunc g_packet_handler[HANDLER_MAX];

enum
{
	PKT_S_ENTERGAME = 1000,
	PKT_S_LEAVEGAME = 1001,
	PKT_S_SPAWN = 1002,
	PKT_S_DESPAWN = 1003,
	PKT_C_MOVE = 1004,
	PKT_S_MOVE = 1005,
	PKT_C_SKILL = 1006,
	PKT_S_SKILL = 1007,
	PKT_S_CHANGEHP = 1008,
	PKT_S_DIE = 1009,
	PKT_S_CONNECTED = 1010,
	PKT_C_LOGIN = 1011,
	PKT_C_CREATEPLAYER = 1012,
	PKT_S_CREATEPLAYER = 1013,
	PKT_C_ENTERGAME = 1014,
	PKT_S_LOGIN = 1015,
	PKT_S_ITEMLIST = 1016,
	PKT_S_ADDITEM = 1017,
	PKT_C_EQUIPITEM = 1018,
	PKT_S_EQUIPITEM = 1019,
	PKT_S_CHANGESTAT = 1020,
	PKT_S_PING = 1021,
	PKT_C_PONG = 1022,
};

bool Handle_INVALID(SharedPacketSession& session, BYTE* buffer, int32 length);
bool Handle_S_ENTERGAME(SharedPacketSession& session, Protocol::S_ENTERGAME& pkt);
bool Handle_S_LEAVEGAME(SharedPacketSession& session, Protocol::S_LEAVEGAME& pkt);
bool Handle_S_SPAWN(SharedPacketSession& session, Protocol::S_SPAWN& pkt);
bool Handle_S_DESPAWN(SharedPacketSession& session, Protocol::S_DESPAWN& pkt);
bool Handle_S_MOVE(SharedPacketSession& session, Protocol::S_MOVE& pkt);
bool Handle_S_SKILL(SharedPacketSession& session, Protocol::S_SKILL& pkt);
bool Handle_S_CHANGEHP(SharedPacketSession& session, Protocol::S_CHANGEHP& pkt);
bool Handle_S_DIE(SharedPacketSession& session, Protocol::S_DIE& pkt);
bool Handle_S_CONNECTED(SharedPacketSession& session, Protocol::S_CONNECTED& pkt);
bool Handle_S_CREATEPLAYER(SharedPacketSession& session, Protocol::S_CREATEPLAYER& pkt);
bool Handle_S_LOGIN(SharedPacketSession& session, Protocol::S_LOGIN& pkt);
bool Handle_S_ITEMLIST(SharedPacketSession& session, Protocol::S_ITEMLIST& pkt);
bool Handle_S_ADDITEM(SharedPacketSession& session, Protocol::S_ADDITEM& pkt);
bool Handle_S_EQUIPITEM(SharedPacketSession& session, Protocol::S_EQUIPITEM& pkt);
bool Handle_S_CHANGESTAT(SharedPacketSession& session, Protocol::S_CHANGESTAT& pkt);
bool Handle_S_PING(SharedPacketSession& session, Protocol::S_PING& pkt);

class ServerPacketHandler
{
public:
	static void Init()
	{
		for (int i = 0; i < HANDLER_MAX; ++i)
			g_packet_handler[i] = Handle_INVALID;

		g_packet_handler[PKT_S_ENTERGAME] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_ENTERGAME>(Handle_S_ENTERGAME, session, buffer, length); };
		g_packet_handler[PKT_S_LEAVEGAME] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_LEAVEGAME>(Handle_S_LEAVEGAME, session, buffer, length); };
		g_packet_handler[PKT_S_SPAWN] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_SPAWN>(Handle_S_SPAWN, session, buffer, length); };
		g_packet_handler[PKT_S_DESPAWN] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_DESPAWN>(Handle_S_DESPAWN, session, buffer, length); };
		g_packet_handler[PKT_S_MOVE] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_MOVE>(Handle_S_MOVE, session, buffer, length); };
		g_packet_handler[PKT_S_SKILL] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_SKILL>(Handle_S_SKILL, session, buffer, length); };
		g_packet_handler[PKT_S_CHANGEHP] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_CHANGEHP>(Handle_S_CHANGEHP, session, buffer, length); };
		g_packet_handler[PKT_S_DIE] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_DIE>(Handle_S_DIE, session, buffer, length); };
		g_packet_handler[PKT_S_CONNECTED] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_CONNECTED>(Handle_S_CONNECTED, session, buffer, length); };
		g_packet_handler[PKT_S_CREATEPLAYER] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_CREATEPLAYER>(Handle_S_CREATEPLAYER, session, buffer, length); };
		g_packet_handler[PKT_S_LOGIN] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_LOGIN>(Handle_S_LOGIN, session, buffer, length); };
		g_packet_handler[PKT_S_ITEMLIST] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_ITEMLIST>(Handle_S_ITEMLIST, session, buffer, length); };
		g_packet_handler[PKT_S_ADDITEM] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_ADDITEM>(Handle_S_ADDITEM, session, buffer, length); };
		g_packet_handler[PKT_S_EQUIPITEM] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_EQUIPITEM>(Handle_S_EQUIPITEM, session, buffer, length); };
		g_packet_handler[PKT_S_CHANGESTAT] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_CHANGESTAT>(Handle_S_CHANGESTAT, session, buffer, length); };
		g_packet_handler[PKT_S_PING] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_PING>(Handle_S_PING, session, buffer, length); };		
	}

	static bool HandlePacket(SharedPacketSession& session, BYTE* buffer, int32 length);	

	static SharedSendBuffer MakeSendBuffer(Protocol::C_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_C_MOVE); }
	static SharedSendBuffer MakeSendBuffer(Protocol::C_SKILL& pkt) { return MakeSendBuffer(pkt, PKT_C_SKILL); }
	static SharedSendBuffer MakeSendBuffer(Protocol::C_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_C_LOGIN); }
	static SharedSendBuffer MakeSendBuffer(Protocol::C_CREATEPLAYER& pkt) { return MakeSendBuffer(pkt, PKT_C_CREATEPLAYER); }
	static SharedSendBuffer MakeSendBuffer(Protocol::C_ENTERGAME& pkt) { return MakeSendBuffer(pkt, PKT_C_ENTERGAME); }
	static SharedSendBuffer MakeSendBuffer(Protocol::C_EQUIPITEM& pkt) { return MakeSendBuffer(pkt, PKT_C_EQUIPITEM); }
	static SharedSendBuffer MakeSendBuffer(Protocol::C_PONG& pkt) { return MakeSendBuffer(pkt, PKT_C_PONG); }	

private:
	template<typename PacketType, typename ProcessFunc>
	static bool ParsePacket(ProcessFunc func, SharedPacketSession& session, BYTE* buffer, int32 length)
	{
		PacketType pkt;
		// temp check
		if (false == pkt.ParseFromArray(buffer + sizeof(PacketHeader), length - sizeof(PacketHeader)))
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static SharedSendBuffer MakeSendBuffer(T& pkt, uint16 packet_id)
	{
		const uint16 data_size = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packet_size = data_size + sizeof(PacketHeader);

		SharedSendBuffer send_buffer = g_send_buffer_manager->Open(packet_size);

		PacketHeader* header = reinterpret_cast<PacketHeader*>(send_buffer->Buffer());
		header->size = packet_size;
		header->id = packet_id;

		// temp check
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], data_size));

		send_buffer->Close(packet_size);

		return send_buffer;
	}
};