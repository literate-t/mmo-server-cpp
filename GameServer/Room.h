#pragma once
#include "JobQueue.h"

class Zone;
class Room : public JobQueue
{
public:
//private:
	// 잡큐를 이용해 싱글 스레드 환경인 것처럼 작업한다
	void Enter(SharedObject object);
	void Leave(SharedObject object);
	void Broatcast(SharedSendBuffer send_buffer);

	// Zone
	Zone GetZone(Vector2Int cell_pos);

	void Init(int32 map_id, int32 zone_cell_size);
	int16 RoomId = 0;

private:
	map<uint64, SharedPlayer> _players;
};