#include "pch.h"
#include "ObjectManager.h"

int32 ObjectManager::GenerateId(GameObjectType type)
{
	WRITE_LOCK;
	return (int32)type << 24 | _counter++;
}

GameObjectType ObjectManager::GetObjectTypeById(int32 object_id)
{
	// 0x7F로 마스킹해 결과를 보장한다
	return GameObjectType((object_id >> 24) & 0x7F);
}
