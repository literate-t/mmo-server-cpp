#pragma once

#include "BufferWriter.h"
#include "BufferReader.h"
#include "Protocol.pb.h"

constexpr int32 HANDLER_MAX = 0x2000;
using PacketHandlerFunc = std::function<bool(SharedPacketSession&, BYTE*, int32)>;
extern PacketHandlerFunc g_packet_handler[HANDLER_MAX];

enum
{
	// TODO: automation
	PKT_S_TEST = 1,
	PKT_S_LOGIN = 2,
};

bool Handle_INVALID(SharedPacketSession& session, BYTE* buffer, int32 length);
// TODO: automation
bool Handle_S_TEST(SharedPacketSession& session, Protocol::S_TEST& pkt);

class ServerPacketHandler
{
public:
	static void Init()
	{
		for (int i = 0; i < HANDLER_MAX; ++i)
			g_packet_handler[i] = Handle_INVALID;

		// TODO: automation
		g_packet_handler[PKT_S_TEST] = [](SharedPacketSession& session, BYTE* buffer, int32 length) { return HandlePacket<Protocol::S_TEST>(Handle_S_TEST, session, buffer, length); };
	}

	static bool HandlePacket(SharedPacketSession& session, BYTE* buffer, int32 length);

	// TODO : automation
	static SharedSendBuffer MakeSendBuffer(Protocol::S_TEST& pkt);

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
	static SharedSendBuffer _MakeSendBuffer(T& pkt, uint16 packet_id)
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