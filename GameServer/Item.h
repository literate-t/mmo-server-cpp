#pragma once
#include "Protocol.pb.h"
#include "DataModel.h"
#include "DataManager.h"
using namespace DataModel;

class Item
{
public:
	Item(ItemType item_type);

	int32 GetItemDbId() const { return _item_info.itemdbid(); }
	void SetItemDbId(int32 item_db_id) { _item_info.set_itemdbid(item_db_id); }
	
	int32 GetDataSheetId() const { return _item_info.datasheetid(); }
	void SetDataSheetId(int32 data_sheed_id) { _item_info.set_datasheetid(data_sheed_id); }

	int32 GetCount() const { return _item_info.count(); }
	void SetCount(int32 count) { _item_info.set_count(count); }

	int32 GetSlot() const { return _item_info.slot(); }
	void SetSlot(int32 slot) { _item_info.set_slot(slot); }

	bool GetEquipped() const { return _item_info.equipped(); }
	void SetEquipped(bool equipped) { _item_info.set_equipped(equipped); }

	Protocol::ItemType GetItemType() const { return _item_type; }

	bool GetStackable() const { return _stackable; }
	void SetStackable(bool stackable) { _stackable = stackable; }

	static shared_ptr<Item> MakeItem(ItemDB item_db);

	const Protocol::ItemInfo& GetItemInfo() { return _item_info; }

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
	Protocol::WeaponType GetWeaponType() const { return _weapon_type; }
	void SetWeaponType(Protocol::WeaponType weapon_type) { _weapon_type = weapon_type; }

	int32 GetDamage() const { return _damage; }
	void SetDamage(int32 damage) { _damage = damage; }

private:
	Protocol::WeaponType _weapon_type;
	int32 _damage;
};

class Armor : public Item
{
public:
	Armor(int32 data_sheet_id);
	void Init(int32 data_sheet_id);

	Protocol::ArmorType GetArmorType() const { return _armor_type; }
	void SetArmorType(Protocol::ArmorType armor_type) { _armor_type = armor_type; }

	int32 GetDefence() const { return _defence; }
	void SetDefence(int32 defence) { _defence = defence; }

private:
	Protocol::ArmorType _armor_type;
	int32 _defence;
};

class Consumable : public Item
{
public:
	Consumable(int32 data_sheet_id);
	void Init(int32 data_sheet_id);

	Protocol::ConsumableType GetConsumableType() const { return _consumable_type; }
	void SetConsumableType(Protocol::ConsumableType consumable_type) { _consumable_type = consumable_type; }

	int32 GetDamage() const { return _damage; }
	void SetDamage(int32 damage) { _damage = damage; }

	int32 GetMaxCount() const { return _max_count; }
	void SetMaxCount(int32 max_count) { _max_count = max_count; }

private:
	Protocol::ConsumableType _consumable_type;
	int32 _damage;
	int32 _max_count;
};

