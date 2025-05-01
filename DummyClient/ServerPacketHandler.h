#pragma once

#include "Protocol.pb.h"

constexpr int32 HANDLER_MAX = 0x2000;
using PacketHandlerFunc = std::function<bool(SharedPacketSession&, BYTE*, int32)>;
extern PacketHandlerFunc g_packet_handler[HANDLER_MAX];

enum
{
	PKT_S_EnterGame = 0,
	PKT_S_LeaveGame = 1,
	PKT_S_Spawn = 2,
	PKT_S_Despawn = 3,
	PKT_C_Move = 4,
	PKT_S_Move = 5,
	PKT_C_Skill = 6,
	PKT_S_Skill = 7,
	PKT_S_ChangeHp = 8,
	PKT_S_Die = 9,
	PKT_S_Connected = 10,
	PKT_C_Login = 11,
	PKT_S_Login = 12,
	PKT_C_EnterGame = 13,
	PKT_S_CreatePlayer = 14,
	PKT_C_CreatePlayer = 15,
	PKT_S_ItemList = 16,
	PKT_S_AddItem = 17,
	PKT_C_EquipItem = 18,
	PKT_S_EquipItem = 19,
	PKT_C_UseItem = 20,
	PKT_S_UseItem = 21,
	PKT_C_DropItem = 22,
	PKT_S_DropItem = 23,
	PKT_S_ChangeStat = 24,
	PKT_C_Pong = 25,
	PKT_S_Ping = 26,
	PKT_C_AnimEnd = 27,
	PKT_S_OnDamaged = 28,
};

bool Handle_INVALID(SharedPacketSession& session, BYTE* buffer, int32 length);
bool Handle_S_EnterGame(SharedPacketSession& session, Protocol::S_EnterGame& pkt);
bool Handle_S_LeaveGame(SharedPacketSession& session, Protocol::S_LeaveGame& pkt);
bool Handle_S_Spawn(SharedPacketSession& session, Protocol::S_Spawn& pkt);
bool Handle_S_Despawn(SharedPacketSession& session, Protocol::S_Despawn& pkt);
bool Handle_S_Move(SharedPacketSession& session, Protocol::S_Move& pkt);
bool Handle_S_Skill(SharedPacketSession& session, Protocol::S_Skill& pkt);
bool Handle_S_ChangeHp(SharedPacketSession& session, Protocol::S_ChangeHp& pkt);
bool Handle_S_Die(SharedPacketSession& session, Protocol::S_Die& pkt);
bool Handle_S_Connected(SharedPacketSession& session, Protocol::S_Connected& pkt);
bool Handle_S_Login(SharedPacketSession& session, Protocol::S_Login& pkt);
bool Handle_S_CreatePlayer(SharedPacketSession& session, Protocol::S_CreatePlayer& pkt);
bool Handle_S_ItemList(SharedPacketSession& session, Protocol::S_ItemList& pkt);
bool Handle_S_AddItem(SharedPacketSession& session, Protocol::S_AddItem& pkt);
bool Handle_S_EquipItem(SharedPacketSession& session, Protocol::S_EquipItem& pkt);
bool Handle_S_UseItem(SharedPacketSession& session, Protocol::S_UseItem& pkt);
bool Handle_S_DropItem(SharedPacketSession& session, Protocol::S_DropItem& pkt);
bool Handle_S_ChangeStat(SharedPacketSession& session, Protocol::S_ChangeStat& pkt);
bool Handle_S_Ping(SharedPacketSession& session, Protocol::S_Ping& pkt);
bool Handle_S_OnDamaged(SharedPacketSession& session, Protocol::S_OnDamaged& pkt);

class ServerPacketHandler
{
public:
	static void Init()
	{
		for (int i = 0; i < HANDLER_MAX; ++i)
			g_packet_handler[i] = Handle_INVALID;

		g_packet_handler[PKT_S_EnterGame] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_EnterGame>(Handle_S_EnterGame, session, buffer, length); };
		g_packet_handler[PKT_S_LeaveGame] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_LeaveGame>(Handle_S_LeaveGame, session, buffer, length); };
		g_packet_handler[PKT_S_Spawn] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_Spawn>(Handle_S_Spawn, session, buffer, length); };
		g_packet_handler[PKT_S_Despawn] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_Despawn>(Handle_S_Despawn, session, buffer, length); };
		g_packet_handler[PKT_S_Move] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_Move>(Handle_S_Move, session, buffer, length); };
		g_packet_handler[PKT_S_Skill] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_Skill>(Handle_S_Skill, session, buffer, length); };
		g_packet_handler[PKT_S_ChangeHp] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_ChangeHp>(Handle_S_ChangeHp, session, buffer, length); };
		g_packet_handler[PKT_S_Die] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_Die>(Handle_S_Die, session, buffer, length); };
		g_packet_handler[PKT_S_Connected] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_Connected>(Handle_S_Connected, session, buffer, length); };
		g_packet_handler[PKT_S_Login] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_Login>(Handle_S_Login, session, buffer, length); };
		g_packet_handler[PKT_S_CreatePlayer] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_CreatePlayer>(Handle_S_CreatePlayer, session, buffer, length); };
		g_packet_handler[PKT_S_ItemList] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_ItemList>(Handle_S_ItemList, session, buffer, length); };
		g_packet_handler[PKT_S_AddItem] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_AddItem>(Handle_S_AddItem, session, buffer, length); };
		g_packet_handler[PKT_S_EquipItem] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_EquipItem>(Handle_S_EquipItem, session, buffer, length); };
		g_packet_handler[PKT_S_UseItem] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_UseItem>(Handle_S_UseItem, session, buffer, length); };
		g_packet_handler[PKT_S_DropItem] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_DropItem>(Handle_S_DropItem, session, buffer, length); };
		g_packet_handler[PKT_S_ChangeStat] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_ChangeStat>(Handle_S_ChangeStat, session, buffer, length); };
		g_packet_handler[PKT_S_Ping] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_Ping>(Handle_S_Ping, session, buffer, length); };
		g_packet_handler[PKT_S_OnDamaged] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_OnDamaged>(Handle_S_OnDamaged, session, buffer, length); };		
	}

	static bool HandlePacket(SharedPacketSession& session, BYTE* buffer, int32 length);	

	static SharedSendBuffer MakeSendBuffer(Protocol::C_Move& pkt) { return MakeSendBuffer(pkt, PKT_C_Move); }
	static SharedSendBuffer MakeSendBuffer(Protocol::C_Skill& pkt) { return MakeSendBuffer(pkt, PKT_C_Skill); }
	static SharedSendBuffer MakeSendBuffer(Protocol::C_Login& pkt) { return MakeSendBuffer(pkt, PKT_C_Login); }
	static SharedSendBuffer MakeSendBuffer(Protocol::C_EnterGame& pkt) { return MakeSendBuffer(pkt, PKT_C_EnterGame); }
	static SharedSendBuffer MakeSendBuffer(Protocol::C_CreatePlayer& pkt) { return MakeSendBuffer(pkt, PKT_C_CreatePlayer); }
	static SharedSendBuffer MakeSendBuffer(Protocol::C_EquipItem& pkt) { return MakeSendBuffer(pkt, PKT_C_EquipItem); }
	static SharedSendBuffer MakeSendBuffer(Protocol::C_UseItem& pkt) { return MakeSendBuffer(pkt, PKT_C_UseItem); }
	static SharedSendBuffer MakeSendBuffer(Protocol::C_DropItem& pkt) { return MakeSendBuffer(pkt, PKT_C_DropItem); }
	static SharedSendBuffer MakeSendBuffer(Protocol::C_Pong& pkt) { return MakeSendBuffer(pkt, PKT_C_Pong); }
	static SharedSendBuffer MakeSendBuffer(Protocol::C_AnimEnd& pkt) { return MakeSendBuffer(pkt, PKT_C_AnimEnd); }	

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