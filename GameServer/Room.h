#pragma once

#include "JobQueue.h"

class Room : public JobQueue
{
public:
//private:
	// 잡큐를 이용해 싱글 스레드 환경인 것처럼 작업한다
	void Enter(SharedPlayer player);
	void Leave(SharedPlayer player);
	void Broatcast(SharedSendBuffer send_buffer);

private:
	map<uint64, SharedPlayer> _players;
};