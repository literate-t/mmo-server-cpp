#pragma once
#include "Protocol.pb.h"
using namespace Protocol;

enum MonsterDataSheetId
{
	GOBLIN_MACE = 1,
	GOBLIN_SPEAR,
	GOBLIN_THIEF,
	ORC_CHIEF,
	ORC_GRUNT
};

struct StatData
{
	int32 level;
	int32 hp;
	int32 max_hp;
	int32 attack;
	float speed;
	int32 total_exp;
};

struct ProjectileData
{
	string name;
	float speed;
	int32 range;
	string prefab;
};

struct SkillData
{
	int32 id;
	string name;
	float coolDown;
	int32 damage;
	SkillType skillType;
	ProjectileData projectile;
};

struct RewardData // 어떤 아이템을 특정 확률로 몇 개를 준다
{
	int32 probability;
	int32 itemId;
	int32 count;
};


struct MonsterData
{
	int32 id;
	string name;
	StatData stat;
	xvector<RewardData> rewards;
};

struct ItemData
{
	int32 id;
	string name;
	ItemType itemType;
};

struct WeaponData : public ItemData
{
	WeaponType weaponType;
	int32 damage;
};

struct ArmorData : public ItemData
{
	ArmorType armorType;
	int32 defence;
};

struct ConsumableData : public ItemData
{
	ConsumableType consumableType;
	int32 maxCount;
	int32 damage;
};
