#pragma once
#include "Session.h"
#include "Protocol.pb.h"
using namespace Protocol;

enum
{
	SEND_TICK = 20,
	SEND_BYTE = 0x400,
};

class GameSession : public PacketSession
{
public:
	~GameSession();	

	void OnConnectCompleted() override;
	void OnDisconnectCompleted() override;
	void OnRecvPacketProcessed(BYTE* buffer, int32 len) override;
	void OnSendCompleted(int32 length) override;

	void ReserveSend(SharedSendBuffer send_buffer);
	void FlushSend() override;
	bool CanFlush();

	void AddPlayer(SharedPlayer player);
	void RemovePlayer(SharedPlayer player);

	int32 GetAccountId() const { return _account_id; }

	inline SharedPlayer& GetPlayer(const int64 index) { return _players[index]; }
	inline SharedPlayer& GetPlayer() { return _current_player; }
	inline weak_ptr<class Room>& GetRoom() { return _room; }

	void HandleLogin(const C_Login& login_packet);
	void HandleCreatePlayer(const C_CreatePlayer& packet);
	void HandleEnterGame(const C_EnterGame& packet);

private:
	void ClearLobbyPlayer();

public:
	uint64 HeartbeatTick = 0;

private:
	xvector<SharedPlayer> _players;
	SharedPlayer _current_player;
	weak_ptr<class Room> _room;
	int32 _account_id;

	PlayerServerState _server_state = PlayerServerState::SERVER_STATE_LOGIN;
	xvector<LobbyPlayerInfo*> _lobby_players;

	uint32 _pending_bytes = 0;
	Atomic<bool> _can_flush = true;

	xqueue<SharedSendBuffer> _send_packets;

	USE_LOCK;
};
