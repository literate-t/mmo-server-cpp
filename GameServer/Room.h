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

private:
	map<uint64, SharedPlayer> _players;
};