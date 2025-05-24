#pragma once
#include "GameObject.h"
#include "Info.h"

class Monster : public GameObject
{
public:
	Monster();
	Monster(string prefab);
	~Monster();

	void Init(MonsterDataSheetId data_sheet_id, string_view prefab);
	void Update() override;
	virtual void OnDead(SharedObject attacker) override;

	const string& GetPrefab() const;

protected:
	virtual void UpdateIdle();
	virtual void UpdateMoving();
	virtual void UpdateSkill();
	virtual void UpdateDead();
	
private:
	void BroadcastState();
	void StopTargeting();
	optional<RewardData> GetRandomReward();	

private:
	int32 _data_sheet_id;
	StatData* _stat_data;
	SkillData* _skill_data;
	string _prefab;
	int16 _search_range = 10;
	int16 _chase_range = 20;
	int16 _skill_range = 1;

	uint64 _skill_tick = 0;
	uint64 _search_tick = 0;
	uint64 _move_tick = 0;

	SharedPlayer _target = nullptr;
	SharedRoom _room;

	SharedJob _job_reserved;
};
