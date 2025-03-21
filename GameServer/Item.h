#pragma once
#include "Protocol.pb.h"
#include "DataModel.h"
#include "DataManager.h"
using namespace DataModel;

class Item
{
public:
	int32 ItemDbId() const { return _item_info.itemdbid(); }
	void ItemDbId(int32 item_db_id) { _item_info.set_itemdbid(item_db_id); }
	
	int32 DataSheetId() const { return _item_info.datasheetid(); }
	void DataSheetId(int32 data_sheed_id) { _item_info.set_datasheetid(data_sheed_id); }

	int32 Count() const { return _item_info.count(); }
	void Count(int32 count) { _item_info.set_count(count); }

	int32 Slot() const { return _item_info.slot(); }
	void Slot(int32 slot) { _item_info.set_slot(slot); }

	bool Equipped() const { return _item_info.equipped(); }
	void Equipped(bool equipped) { _item_info.set_equipped(equipped); }

	Protocol::ItemType ItemType() const { return _item_type; }

	bool Stackable() const { return _stackable; }
	void Stackable(bool stackable) { _stackable = stackable; }

	static shared_ptr<Item> MakeItem(ItemDB item_db);	

protected:
	Protocol::ItemType _item_type;
	bool _stackable;

private:
	Protocol::ItemInfo _item_info;
};

class Weapon : public Item
{
public:
	Weapon(int32 data_sheet_id);

	void Init(int32 data_sheet_id);
	Protocol::WeaponType WeaponType() const { return _weapon_type; }
	void WeaponType(Protocol::WeaponType weapon_type) { _weapon_type = weapon_type; }

	int32 Damage() const { return _damage; }
	void Damage(int32 damage) { _damage = damage; }

private:
	Protocol::WeaponType _weapon_type;
	int32 _damage;
};

class Armor : public Item
{
public:
	Armor(int32 data_sheet_id);
	void Init(int32 data_sheet_id);

	Protocol::ArmorType ArmorType() const { return _armor_type; }
	void ArmorType(Protocol::ArmorType armor_type) { _armor_type = armor_type; }

	int32 Defence() const { return _defence; }
	void Defence(int32 defence) { _defence = defence; }

private:
	Protocol::ArmorType _armor_type;
	int32 _defence;
};

class Consumable : public Item
{
public:
	Consumable(int32 data_sheet_id);
	void Init(int32 data_sheet_id);

	Protocol::ConsumableType ConsumableType() const { return _consumable_type; }
	void ConsumableType(Protocol::ConsumableType consumable_type) { _consumable_type = consumable_type; }

	int32 Damage() const { return _damage; }
	void Damage(int32 damage) { _damage = damage; }

	int32 MaxCount() const { return _max_count; }
	void MaxCount(int32 max_count) { _max_count = max_count; }

private:
	Protocol::ConsumableType _consumable_type;
	int32 _damage;
	int32 _max_count;
};

