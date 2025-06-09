#include "pch.h"
#include "Item.h"

Weapon::Weapon(int32 data_sheet_id)
	:Item(ItemType::ITEM_TYPE_WEAPON)
{
	Init(data_sheet_id);
}

void Weapon::Init(int32 data_sheet_id)
{
	ItemData* item_data = g_data_manager->Item(data_sheet_id);
	WeaponData* data = reinterpret_cast<WeaponData*>(item_data);	
	
	SetDataSheetId(data->id);
	SetCount(1);
	SetWeaponType(data->weaponType);
	SetDamage(data->damage);
	SetStackable(false);
}

Armor::Armor(int32 data_sheet_id)
	:Item(ItemType::ITEM_TYPE_ARMOR)
{
	Init(data_sheet_id);
}

void Armor::Init(int32 data_sheet_id)
{
	ItemData* item_data = g_data_manager->Item(data_sheet_id);
	ArmorData* data = reinterpret_cast<ArmorData*>(item_data);

	SetDataSheetId(data->id);
	SetCount(1);
	SetDefence(data->defence);
	SetArmorType(data->armorType);	
	SetStackable(false);
}

Consumable::Consumable(int32 data_sheet_id)
	:Item(ItemType::ITEM_TYPE_CONSUMABLE)
{
	Init(data_sheet_id);
}

void Consumable::Init(int32 data_sheet_id)
{
	ItemData* item_data = g_data_manager->Item(data_sheet_id);
	ConsumableData* data = reinterpret_cast<ConsumableData*>(item_data);

	SetDataSheetId(data->id);
	SetCount(1);
	SetMaxCount(data->maxCount);
	SetDamage(data->damage);
	SetConsumableType(data->consumableType);
	SetStackable(data->maxCount > 1);
}

Item::Item(ItemType item_type)
: _item_type(item_type)
{
}

shared_ptr<Item> Item::MakeItem(ItemDB item_db)
{
	shared_ptr<Item> item = nullptr;
	const ItemData* item_data = g_data_manager->Item(item_db.DataSheetId);

	switch (item_data->itemType)
	{
	case ItemType::ITEM_TYPE_WEAPON:
		item = MakeShared<Weapon>(item_db.DataSheetId);
		break;
	case ItemType::ITEM_TYPE_ARMOR:
		item = MakeShared<Armor>(item_db.DataSheetId);
		break;
	case ItemType::ITEM_TYPE_CONSUMABLE:
		item = MakeShared<Consumable>(item_db.DataSheetId);
		break;
	}

	item->SetItemDbId(item_db.ItemDbId);
	item->SetDataSheetId(item_db.DataSheetId);
	item->SetCount(item_db.Count);
	item->SetSlot(item_db.Slot);
	item->SetEquipped(item_db.Equipped);

	return item;
}
