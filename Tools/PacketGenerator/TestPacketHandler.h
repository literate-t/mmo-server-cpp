#pragma once

#include "Protocol.pb.h"

constexpr int32 HANDLER_MAX = 0x2000;
using PacketHandlerFunc = std::function<bool(SharedPacketSession&, BYTE*, int32)>;
extern PacketHandlerFunc g_packet_handler[HANDLER_MAX];

enum
{
	// TODO: automation
	PKT_C_TEST = 1000,
	PKT_S_TEST = 1001,
	PKT_S_LOGIN = 1002,
};

bool Handle_INVALID(SharedPacketSession& session, BYTE* buffer, int32 length);

// TODO: automation
bool Handle_C_TEST(SharedPacketSession& session, Protocol::C_TEST& pkt);

class TestPacketHandler
{
public:
	static void Init()
	{
		for (int i = 0; i < HANDLER_MAX; ++i)
			g_packet_handler[i] = Handle_INVALID;

		// TODO: automation
		g_packet_handler[PKT_C_TEST] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return HandlePacket<Protocol::C_TEST>(Handle_C_TEST, session, buffer, length); };		
	}

	static bool HandlePacket(SharedPacketSession& session, BYTE* buffer, int32 length);

	// TODO : automation
	static SharedSendBuffer MakeSendBuffer(Protocol::S_TEST&pkt) { return MakeSendBuffer(pkt, PKT_S_TEST); }
	static SharedSendBuffer MakeSendBuffer(Protocol::S_LOGIN&pkt) { return MakeSendBuffer(pkt, PKT_S_LOGIN); }	

private:

	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, SharedPacketSession& session, BYTE* buffer, int32 length)
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