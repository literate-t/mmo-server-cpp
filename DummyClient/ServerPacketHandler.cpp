#include "pch.h"
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "Protocol.pb.h"

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

bool Handle_S_LOGIN(SharedPacketSession& session, Protocol::S_LOGIN& pkt)
{
	if (false == pkt.success())
		return false;

	if (0 == pkt.player().size())
	{
		// ĳ���� ���� â
	}

	Protocol::C_ENTER_GAME enter_game_pkt;
	enter_game_pkt.set_player_index(0); // �ϴ��� ù ��° ĳ���ͷ� ����
	auto send_buffer = ServerPacketHandler::MakeSendBuffer(enter_game_pkt);

	session->Send(send_buffer);

	return true;
}

bool Handle_S_ENTER_GAME(SharedPacketSession& session, Protocol::S_ENTER_GAME& pkt)
{
	// TODO: ������ �����ϰ� ��Ī������ Room�̶� Player ������ ������ִ� �� ����
	return true;
}

bool Handle_S_CHAT(SharedPacketSession& session, Protocol::S_CHAT& pkt)
{	
	printf("client: %s\n", pkt.msg().c_str());
	return true;
}
