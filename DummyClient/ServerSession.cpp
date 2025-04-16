#include "pch.h"
#include "ServerSession.h"
#include "ServerPacketHandler.h"
#include "ServerSessionManager.h"

ServerSession::ServerSession()
: _dummy_id(ServerSessionManager::Instance()->GenerateDummyId())
{

}

ServerSession::~ServerSession()
{
	puts("~ServerSession");
}

void ServerSession::OnConnectCompleted()
{
}

void ServerSession::OnDisconnectCompleted()
{
}

void ServerSession::OnRecvPacketProcessed(BYTE* buffer, int32 length)
{
	SharedPacketSession session = GetSharedPacketSession();
	ServerPacketHandler::HandlePacket(session, buffer, length);
}

void ServerSession::OnSendCompleted(int32 length)
{
}
