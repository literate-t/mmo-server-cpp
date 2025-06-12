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
	PKT_C_GoRandom = 29,
};

bool Handle_INVALID(SharedPacketSession& session, BYTE* buffer, int32 length);
bool Handle_C_Move(SharedPacketSession& session, const Protocol::C_Move& pkt);
bool Handle_C_Skill(SharedPacketSession& session,const Protocol::C_Skill& pkt);
bool Handle_C_Login(SharedPacketSession& session, const Protocol::C_Login& pkt);
bool Handle_C_EnterGame(SharedPacketSession& session, const Protocol::C_EnterGame& pkt);
bool Handle_C_CreatePlayer(SharedPacketSession& session, const Protocol::C_CreatePlayer& pkt);
bool Handle_C_EquipItem(SharedPacketSession& session, const Protocol::C_EquipItem& pkt);
bool Handle_C_UseItem(SharedPacketSession& session, const Protocol::C_UseItem& pkt);
bool Handle_C_DropItem(SharedPacketSession& session, const Protocol::C_DropItem& pkt);
bool Handle_C_Pong(SharedPacketSession& session, const Protocol::C_Pong& pkt);
bool Handle_C_AnimEnd(SharedPacketSession& session, const Protocol::C_AnimEnd& pkt);
bool Handle_C_GoRandom(SharedPacketSession& session, const Protocol::C_GoRandom& pkt);

class ClientPacketHandler
{
public:
	static void Init()
	{
		for (int i = 0; i < HANDLER_MAX; ++i)
			g_packet_handler[i] = Handle_INVALID;

		g_packet_handler[PKT_C_Move] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::C_Move>(Handle_C_Move, session, buffer, length); };
		g_packet_handler[PKT_C_Skill] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::C_Skill>(Handle_C_Skill, session, buffer, length); };
		g_packet_handler[PKT_C_Login] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::C_Login>(Handle_C_Login, session, buffer, length); };
		g_packet_handler[PKT_C_EnterGame] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::C_EnterGame>(Handle_C_EnterGame, session, buffer, length); };
		g_packet_handler[PKT_C_CreatePlayer] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::C_CreatePlayer>(Handle_C_CreatePlayer, session, buffer, length); };
		g_packet_handler[PKT_C_EquipItem] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::C_EquipItem>(Handle_C_EquipItem, session, buffer, length); };
		g_packet_handler[PKT_C_UseItem] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::C_UseItem>(Handle_C_UseItem, session, buffer, length); };
		g_packet_handler[PKT_C_DropItem] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::C_DropItem>(Handle_C_DropItem, session, buffer, length); };
		g_packet_handler[PKT_C_Pong] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::C_Pong>(Handle_C_Pong, session, buffer, length); };
		g_packet_handler[PKT_C_AnimEnd] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::C_AnimEnd>(Handle_C_AnimEnd, session, buffer, length); };
		g_packet_handler[PKT_C_GoRandom] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::C_GoRandom>(Handle_C_GoRandom, session, buffer, length); };		
	}

	static bool HandlePacket(SharedPacketSession& session, BYTE* buffer, int32 length);	

	static SharedSendBuffer MakeSendBuffer(Protocol::S_EnterGame& pkt) { return MakeSendBuffer(pkt, PKT_S_EnterGame); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_LeaveGame& pkt) { return MakeSendBuffer(pkt, PKT_S_LeaveGame); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_Spawn& pkt) { return MakeSendBuffer(pkt, PKT_S_Spawn); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_Despawn& pkt) { return MakeSendBuffer(pkt, PKT_S_Despawn); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_Move& pkt) { return MakeSendBuffer(pkt, PKT_S_Move); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_Skill& pkt) { return MakeSendBuffer(pkt, PKT_S_Skill); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_ChangeHp& pkt) { return MakeSendBuffer(pkt, PKT_S_ChangeHp); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_Die& pkt) { return MakeSendBuffer(pkt, PKT_S_Die); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_Connected& pkt) { return MakeSendBuffer(pkt, PKT_S_Connected); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_Login& pkt) { return MakeSendBuffer(pkt, PKT_S_Login); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_CreatePlayer& pkt) { return MakeSendBuffer(pkt, PKT_S_CreatePlayer); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_ItemList& pkt) { return MakeSendBuffer(pkt, PKT_S_ItemList); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_AddItem& pkt) { return MakeSendBuffer(pkt, PKT_S_AddItem); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_EquipItem& pkt) { return MakeSendBuffer(pkt, PKT_S_EquipItem); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_UseItem& pkt) { return MakeSendBuffer(pkt, PKT_S_UseItem); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_DropItem& pkt) { return MakeSendBuffer(pkt, PKT_S_DropItem); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_ChangeStat& pkt) { return MakeSendBuffer(pkt, PKT_S_ChangeStat); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_Ping& pkt) { return MakeSendBuffer(pkt, PKT_S_Ping); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_OnDamaged& pkt) { return MakeSendBuffer(pkt, PKT_S_OnDamaged); }	

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

		SharedSendBuffer send_buffer = SendBufferManager::Instance().Acquire(packet_size);

		PacketHeader* header = reinterpret_cast<PacketHeader*>(send_buffer->Buffer());
		header->size = packet_size;
		header->id = packet_id;

		ASSERT_CRASH(pkt.SerializeToArray(&header[1], data_size));

		return send_buffer;
	}
};