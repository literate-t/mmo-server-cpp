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
		// 캐릭터 생성 창
	}

	Protocol::C_ENTER_GAME enter_game_pkt;
	enter_game_pkt.set_player_index(0); // 일단은 첫 번째 캐릭터로 입장
	auto send_buffer = ServerPacketHandler::MakeSendBuffer(enter_game_pkt);

	session->Send(send_buffer);

	return true;
}

bool Handle_S_ENTER_GAME(SharedPacketSession& session, Protocol::S_ENTER_GAME& pkt)
{
	// TODO: 서버와 동일하게 대칭적으로 Room이랑 Player 정보를 만들어주는 게 좋다
	return true;
}

bool Handle_S_CHAT(SharedPacketSession& session, Protocol::S_CHAT& pkt)
{	
	printf("client: %s\n", pkt.msg().c_str());
	return true;
}
