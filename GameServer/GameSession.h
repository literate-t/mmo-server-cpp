#pragma once
#include "Session.h"

class GameSession : public PacketSession
{
public:
	~GameSession();	

	void OnConnectCompleted() override;
	void OnDisconnectCompleted() override;
	void OnRecvPacketProcessed(BYTE* buffer, int32 len) override;
	void OnSendCompleted(int32 length) override;

	void AddPlayer(SharedPlayer player);
	void RemovePlayer(SharedPlayer player);

	inline SharedPlayer& GetPlayer(const int64 index) { return _players[index]; }
	inline SharedPlayer& GetPlayer() { return _current_player; }
	inline weak_ptr<class Room>& GetRoom() { return _room; }

private:
	xvector<SharedPlayer> _players;
	SharedPlayer _current_player;
	weak_ptr<class Room> _room;
};
