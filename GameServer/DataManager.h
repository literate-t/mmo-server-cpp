#pragma once
#include "Info.h"
#include "nlohmann\json.hpp"
using json = nlohmann::json;

class ConfigManager;
class DataManager
{
public:
	DataManager(ConfigManager* config_manager);

	StatData& Stat(const int16 level) { return _level_stat_table.at(level); }
	SkillData& Skill(const int16 id) { return _skill_table.at(id); }
	MonsterData& Monster(const int16 id) { return _monster_table.at(id); }
	ItemData& Item(const int16 id) { return _item_table.at(id); }

private:
	void LoadConfig();
	void LoadData(string path);

	void MakeStatTable(json& j);
	void MakeSkillTable(json& j);
	void MakeMonsterTable(json& j);
	void MakeItemTable(json& j);	

private:
	ConfigManager* _config_manager;

	xmap<int16, StatData> _level_stat_table;
	xmap<int16, SkillData> _skill_table;
	xmap<int16, MonsterData> _monster_table;
	xmap<int16, ItemData> _item_table;
};

void from_json(const json& j, StatData& stat);
void from_json(const json& j, SkillData& skill);
void from_json(const json& j, ProjectileData& projectile);
void from_json(const json& j, MonsterData& monster);
void from_json(const json& j, RewardData& reward);
void from_json(const json& j, WeaponData& weapon);
void from_json(const json& j, ArmorData& armor);
void from_json(const json& j, ConsumableData& consumable);

SkillType GetSkillTypeEnum(string type);
WeaponType GetWeaponTypeEnum(string type);
ArmorType GetArmorTypeEnum(string type);
ConsumableType GetConsumableTypeEnum(string type);