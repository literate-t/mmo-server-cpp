#pragma once

#include "JobQueue.h"

class Room : public JobQueue
{
public:
//private:
	// ��ť�� �̿��� �̱� ������ ȯ���� ��ó�� �۾��Ѵ�
	void Enter(SharedPlayer player);
	void Leave(SharedPlayer player);
	void Broatcast(SharedSendBuffer send_buffer);

	void Init(int32 map_id, int32 zone_cell_size);
	int16 RoomId = 0;

private:
	map<uint64, SharedPlayer> _players;
};