#include "pch.h"
#include "Arrow.h"
#include "Room.h"
#include "Map.h"
#include "ClientPacketHandler.h"

void Arrow::Update()
{
	SharedRoom room = GetRoom();
	if (Owner == nullptr || room == nullptr || Data == nullptr) return;

	int32 tick = static_cast<int32>(1000 / Data->projectile.speed);
	auto this_shared = shared_from_this();
	room->PushTimerAsync(tick, [this_shared]() { this_shared->Update(); });

	Vector2Int dest_pos = GetFrontCellPosition();
	if (room->GetMap()->ApplyMove(shared_from_this(), dest_pos, false))
	{
		S_Move move;
		move.set_objectid(GetObjectId());
		PositionInfo* pos_info = move.mutable_posinfo();
		pos_info->MergeFrom(GetPositionInfo());
		room->Broadcast(GetCellPos(), ClientPacketHandler::MakeSendBuffer(move));
	}
	else
	{
		SharedObject target = room->GetMap()->Find(dest_pos);
		if (target != nullptr)
			target->OnDamaged(shared_from_this(), Owner->GetTotalAttack());

		room->PushJobAsync(&Room::Leave, GetObjectId());		
	}
}

SharedObject Arrow::GetOwner()
{
	return Owner;
}
