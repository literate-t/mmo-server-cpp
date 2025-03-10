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
bool Handle_C_MOVE(SharedPacketSession& session, Protocol::C_MOVE& pkt);
bool Handle_C_SKILL(SharedPacketSession& session, Protocol::C_SKILL& pkt);
bool Handle_C_LOGIN(SharedPacketSession& session, Protocol::C_LOGIN& pkt);
bool Handle_C_CREATEPLAYER(SharedPacketSession& session, Protocol::C_CREATEPLAYER& pkt);
bool Handle_C_ENTERGAME(SharedPacketSession& session, Protocol::C_ENTERGAME& pkt);
bool Handle_C_EQUIPITEM(SharedPacketSession& session, Protocol::C_EQUIPITEM& pkt);
bool Handle_C_PONG(SharedPacketSession& session, Protocol::C_PONG& pkt);

class ClientPacketHandler
{
public:
	static void Init()
	{
		for (int i = 0; i < HANDLER_MAX; ++i)
			g_packet_handler[i] = Handle_INVALID;

		g_packet_handler[PKT_C_MOVE] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::C_MOVE>(Handle_C_MOVE, session, buffer, length); };
		g_packet_handler[PKT_C_SKILL] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::C_SKILL>(Handle_C_SKILL, session, buffer, length); };
		g_packet_handler[PKT_C_LOGIN] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::C_LOGIN>(Handle_C_LOGIN, session, buffer, length); };
		g_packet_handler[PKT_C_CREATEPLAYER] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::C_CREATEPLAYER>(Handle_C_CREATEPLAYER, session, buffer, length); };
		g_packet_handler[PKT_C_ENTERGAME] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::C_ENTERGAME>(Handle_C_ENTERGAME, session, buffer, length); };
		g_packet_handler[PKT_C_EQUIPITEM] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::C_EQUIPITEM>(Handle_C_EQUIPITEM, session, buffer, length); };
		g_packet_handler[PKT_C_PONG] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::C_PONG>(Handle_C_PONG, session, buffer, length); };		
	}

	static bool HandlePacket(SharedPacketSession& session, BYTE* buffer, int32 length);	

	static SharedSendBuffer MakeSendBuffer(Protocol::S_ENTERGAME& pkt) { return MakeSendBuffer(pkt, PKT_S_ENTERGAME); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_LEAVEGAME& pkt) { return MakeSendBuffer(pkt, PKT_S_LEAVEGAME); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_SPAWN& pkt) { return MakeSendBuffer(pkt, PKT_S_SPAWN); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_DESPAWN& pkt) { return MakeSendBuffer(pkt, PKT_S_DESPAWN); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_S_MOVE); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_SKILL& pkt) { return MakeSendBuffer(pkt, PKT_S_SKILL); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_CHANGEHP& pkt) { return MakeSendBuffer(pkt, PKT_S_CHANGEHP); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_DIE& pkt) { return MakeSendBuffer(pkt, PKT_S_DIE); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_CONNECTED& pkt) { return MakeSendBuffer(pkt, PKT_S_CONNECTED); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_CREATEPLAYER& pkt) { return MakeSendBuffer(pkt, PKT_S_CREATEPLAYER); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_S_LOGIN); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_ITEMLIST& pkt) { return MakeSendBuffer(pkt, PKT_S_ITEMLIST); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_ADDITEM& pkt) { return MakeSendBuffer(pkt, PKT_S_ADDITEM); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_EQUIPITEM& pkt) { return MakeSendBuffer(pkt, PKT_S_EQUIPITEM); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_CHANGESTAT& pkt) { return MakeSendBuffer(pkt, PKT_S_CHANGESTAT); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_PING& pkt) { return MakeSendBuffer(pkt, PKT_S_PING); }	

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