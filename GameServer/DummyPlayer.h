#pragma once
#include "Player.h"
#include "Monster.h"

class DummyPlayer : public Player
{
public:
	DummyPlayer();
	~DummyPlayer();

	void Update() override;
	virtual void OnDead(SharedObject attacker) override;

protected:
	virtual void UpdateIdle();
	virtual void UpdateMoving();
	virtual void UpdateSkill();
	virtual void UpdateDead();

private:
	void BroadcastState();
	void StopTargeting();

private:
	int32 _data_sheet_id;
	StatData* _stat_data;
	SkillData* _skill_data;
	int16 _search_range = 10;
	int16 _chase_range = 20;
	int16 _skill_range = 1;

	uint64 _skill_tick = 0;
	uint64 _search_tick = 0;
	uint64 _move_tick = 0;

	SharedObject _target = nullptr;
	//SharedMonster _target = nullptr;
	SharedRoom _room;

	SharedJob _job_reserved;
};

