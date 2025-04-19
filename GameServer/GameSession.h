#pragma once
#include "Session.h"
#include "Protocol.pb.h"
using namespace Protocol;

enum
{
	SEND_TICK = 10,
	SEND_BYTE = 0x300,
};

class GameSession : public PacketSession
{
public:
	~GameSession();	

	void OnConnectCompleted() override;
	void OnDisconnectCompleted() override;
	void OnRecvPacketProcessed(BYTE* buffer, int32 len) override;
	void OnSendCompleted(int32 length) override;

	void Send(SharedSendBuffer send_buffer);
	void FlushSend();
	bool CanFlush();

	void AddPlayer(SharedPlayer player);
	void RemovePlayer(SharedPlayer player);

	int32 GetAccountId() const { return _account_id; }

	inline SharedPlayer& GetPlayer(const int64 index) { return _players[index]; }
	inline SharedPlayer& GetPlayer() { return _current_player; }
	inline weak_ptr<class Room>& GetRoom() { return _room; }

	void HandleLogin(C_Login& login_packet);
	void HandleCreatePlayer(C_CreatePlayer packet);
	void HandleEnterGame(C_EnterGame packet);

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
