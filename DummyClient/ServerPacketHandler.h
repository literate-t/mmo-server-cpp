#pragma once

#include "Protocol.pb.h"

constexpr int32 HANDLER_MAX = 0x2000;
using PacketHandlerFunc = std::function<bool(SharedPacketSession&, BYTE*, int32)>;
extern PacketHandlerFunc g_packet_handler[HANDLER_MAX];

enum
{
	PKT_C_LOGIN = 1000,
	PKT_S_LOGIN = 1001,
	PKT_C_ENTER_GAME = 1002,
	PKT_S_ENTER_GAME = 1003,
	PKT_C_CHAT = 1004,
	PKT_S_CHAT = 1005,
};

bool Handle_INVALID(SharedPacketSession& session, BYTE* buffer, int32 length);
bool Handle_S_LOGIN(SharedPacketSession& session, Protocol::S_LOGIN& pkt);
bool Handle_S_ENTER_GAME(SharedPacketSession& session, Protocol::S_ENTER_GAME& pkt);
bool Handle_S_CHAT(SharedPacketSession& session, Protocol::S_CHAT& pkt);

class ServerPacketHandler
{
public:
	static void Init()
	{
		for (int i = 0; i < HANDLER_MAX; ++i)
			g_packet_handler[i] = Handle_INVALID;

		g_packet_handler[PKT_S_LOGIN] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_LOGIN>(Handle_S_LOGIN, session, buffer, length); };
		g_packet_handler[PKT_S_ENTER_GAME] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_ENTER_GAME>(Handle_S_ENTER_GAME, session, buffer, length); };
		g_packet_handler[PKT_S_CHAT] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return ParsePacket <Protocol::S_CHAT>(Handle_S_CHAT, session, buffer, length); };		
	}

	static bool HandlePacket(SharedPacketSession& session, BYTE* buffer, int32 length);	

	static SharedSendBuffer MakeSendBuffer(Protocol::C_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_C_LOGIN); }
	static SharedSendBuffer MakeSendBuffer(Protocol::C_ENTER_GAME& pkt) { return MakeSendBuffer(pkt, PKT_C_ENTER_GAME); }
	static SharedSendBuffer MakeSendBuffer(Protocol::C_CHAT& pkt) { return MakeSendBuffer(pkt, PKT_C_CHAT); }	

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