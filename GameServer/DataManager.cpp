#include "pch.h"
#include "DataManager.h"
#include "ConfigManager.h"
#include "JsonFileManager.h"

DataManager::DataManager(ConfigManager* config_manager)
	:_config_manager(config_manager)
{
	LoadConfig();
}

void DataManager::LoadConfig()
{
	string data_path = _config_manager->GetDataPath();
	
	LoadData(data_path);
}

void DataManager::LoadData(string path)
{
	// stat
	string data_path = path + "/StatData.json";
	string data = JsonFileManager::GetContent(data_path);
	json j = json::parse(data);
	MakeStatTable(j);

	// skill
	data_path = path + "/SkillData.json";
	data = JsonFileManager::GetContent(data_path);
	j = json::parse(data);
	MakeSkillTable(j);

	// monster
	data_path = path + "/MonsterData.json";
	data = JsonFileManager::GetContent(data_path);
	j = json::parse(data);
	MakeMonsterTable(j);

	// item
	data_path = path + "/ItemData.json";
	data = JsonFileManager::GetContent(data_path);
	j = json::parse(data);
	MakeItemTable(j);
}

void DataManager::MakeStatTable(json& j)
{
	xvector<StatData> stats = j.at("stats").get<xvector<StatData>>();
	for (auto stat : stats)
	{
		stat.hp = stat.max_hp;
		_level_stat_table[(int16)stat.level] = stat;
	}
}

void DataManager::MakeSkillTable(json& j)
{
	xvector<SkillData> skills = j.at("skills").get<xvector<SkillData>>();
	for (auto skill : skills)
		_skill_table[skill.id] = skill;
}

void DataManager::MakeMonsterTable(json& j)
{
	xvector<MonsterData> monsters = j.at("monsters").get<xvector<MonsterData>>();
	for (auto monster : monsters)
		_monster_table[monster.id] = monster;
}

void DataManager::MakeItemTable(json& j)
{
	xvector<WeaponData> weapons = j.at("weapons").get<xvector<WeaponData>>();
	for (auto weapon : weapons)
	{
		weapon.itemType = ItemType::ITEM_TYPE_WEAPON;
		_weapon_table[weapon.id] = weapon;
	}

	xvector<ArmorData> armors = j.at("armors").get<xvector<ArmorData>>();
	for (auto armor : armors)
	{
		armor.itemType = ItemType::ITEM_TYPE_ARMOR;
		_armor_table[armor.id] = armor;
	}

	xvector<ConsumableData> consumables = j.at("consumables").get<xvector<ConsumableData>>();
	for (auto consumable : consumables)
	{
		consumable.itemType = ItemType::ITEM_TYPE_CONSUMABLE;
		_cosumable_table[consumable.id] = consumable;
	}
}

void from_json(const json& j, StatData& stat)
{
	j.at("level").get_to(stat.level);
	j.at("maxHp").get_to(stat.max_hp);
	j.at("attack").get_to(stat.attack);
	j.at("totalExp").get_to(stat.total_exp);
	j.at("speed").get_to(stat.speed);
}

void from_json(const json& j, SkillData& skill)
{
	j.at("id").get_to(skill.id);
	j.at("name").get_to<string>(skill.name);
	j.at("cooldown").get_to(skill.coolDown);
	j.at("damage").get_to(skill.damage);
	string type = j.at("skillType").get<string>();
	skill.skillType = GetSkillTypeEnum(type);
}

void from_json(const json& j, MonsterData& monster)
{
	j.at("id").get_to(monster.id);
	j.at("name").get_to(monster.name);
	j.at("stat").get_to(monster.stat);
	j.at("rewards").get_to(monster.rewards);
}

void from_json(const json& j, RewardData& reward)
{
	j.at("probability").get_to(reward.probability);
	j.at("itemId").get_to(reward.itemId);
	j.at("count").get_to(reward.count);
}

void from_json(const json& j, WeaponData& weapon)
{
	j.at("id").get_to(weapon.id);
	j.at("name").get_to(weapon.name);
	string type = j.at("weaponType").get<string>();
	weapon.weaponType = GetWeaponTypeEnum(type);
	j.at("damage").get_to(weapon.damage);
}

void from_json(const json& j, ArmorData& armor)
{
	j.at("id").get_to(armor.id);
	j.at("name").get_to(armor.name);
	string type = j.at("armorType").get<string>();
	armor.armorType = GetArmorTypeEnum(type);
	j.at("defence").get_to(armor.defence);
}

void from_json(const json& j, ConsumableData& consumable)
{
	j.at("id").get_to(consumable.id);
	j.at("name").get_to(consumable.name);
	string type = j.at("consumableType").get<string>();
	consumable.consumableType = GetConsumableTypeEnum(type);
	j.at("damage").get_to(consumable.damage);
	j.at("maxCount").get_to(consumable.maxCount);
}

SkillType GetSkillTypeEnum(string type)
{
	if (type == "SkillAuto")
		return SkillType::SKILL_AUTO;
	else if (type == "SkillProjectTile")
		return SkillType::SKILL_PROJECTILE;
	else
		return SkillType::SKILL_NONE;
}

WeaponType GetWeaponTypeEnum(string type)
{
	if (type == "Sword")
		return WeaponType::WEAPON_TYPE_SWORD;
	else if (type == "Bow")
		return WeaponType::WEAPON_TYPE_BOW;
	else
		return WeaponType::WEAPON_TYPE_NONE;
}

ArmorType GetArmorTypeEnum(string type)
{
	if (type == "Chestarmor")
		return ArmorType::ARMOR_TYPE_CHESTARMOR;
	else if (type == "Helmet")
		return ArmorType::ARMOR_TYPE_HELMET;
	else if (type == "Boots")
		return ArmorType::ARMOR_TYPE_BOOTS;
	else
		return ArmorType::ARMOR_TYPE_NONE;
}

ConsumableType GetConsumableTypeEnum(string type)
{
	if (type == "Potion")
		return ConsumableType::CONSUMABLE_TYPE_POTION;
	else
		return ConsumableType::CONSUMABLE_TYPE_NONE;
}
