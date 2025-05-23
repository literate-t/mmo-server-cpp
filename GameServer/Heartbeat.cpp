#include "pch.h"
#include "Heartbeat.h"
#include "GameSession.h"
#include "ClientPacketHandler.h"

void Heartbeat::Ping(SharedGameSession owner)
{
	uint64 diff = GetTickCount64() - owner->HeartbeatTick;
	if (diff > 8000)
	{
		owner->Disconnect(L"PingPong failed");
		owner = nullptr;
		return;
	}

	S_Ping ping;
	owner->Send(ClientPacketHandler::MakeSendBuffer(ping));
	g_shared_heart->PushTimerAsync(5000, &Heartbeat::Ping, owner);
}

void Heartbeat::HandlePong(SharedGameSession owner)
{
	owner->HeartbeatTick = GetTickCount64();
}
