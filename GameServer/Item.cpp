#include "pch.h"
#include "Item.h"

Weapon::Weapon(int32 data_sheet_id)
{
	Init(data_sheet_id);
}

void Weapon::Init(int32 data_sheet_id)
{
	ItemData& item_data = g_data_manager->Item(data_sheet_id);
	WeaponData* data = reinterpret_cast<WeaponData*>(&item_data);	
	
	DataSheetId(data->id);
	Count(1);
	WeaponType(data->weaponType);
	Damage(data->damage);
	Stackable(false);
}

Armor::Armor(int32 data_sheet_id)
{
	Init(data_sheet_id);
}

void Armor::Init(int32 data_sheet_id)
{
	ItemData& item_data = g_data_manager->Item(data_sheet_id);
	ArmorData* data = reinterpret_cast<ArmorData*>(&item_data);

	DataSheetId(data->id);
	Count(1);
	Defence(data->defence);
	ArmorType(data->armorType);	
	Stackable(false);
}

Consumable::Consumable(int32 data_sheet_id)
{
	Init(data_sheet_id);
}

void Consumable::Init(int32 data_sheet_id)
{
	ItemData& item_data = g_data_manager->Item(data_sheet_id);
	ConsumableData* data = reinterpret_cast<ConsumableData*>(&item_data);

	DataSheetId(data->id);
	Count(1);
	MaxCount(data->maxCount);
	Damage(data->damage);
	ConsumableType(data->consumableType);
	Stackable(data->maxCount > 1);
}

shared_ptr<Item> Item::MakeItem(ItemDB item_db)
{
	shared_ptr<Item> item = nullptr;
	const ItemData& item_data = g_data_manager->Item(item_db.DataSheetId);

	switch (item_data.itemType)
	{
	case ItemType::ITEM_TYPE_WEAPON:
		item = make_shared<Weapon>(item_db.DataSheetId);
		break;
	case ItemType::ITEM_TYPE_ARMOR:
		item = make_shared<Armor>(item_db.DataSheetId);
		break;
	case ItemType::ITEM_TYPE_CONSUMABLE:
		item = make_shared<Consumable>(item_db.DataSheetId);
		break;
	}

	item->ItemDbId(item_db.ItemDbId);
	item->DataSheetId(item_db.DataSheetId);
	item->Count(item_db.Count);
	item->Slot(item_db.Slot);
	item->Equipped(item_db.Equipped);

	return item;
}
