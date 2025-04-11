#include "pch.h"
#include "Inventory.h"

void Inventory::Add(SharedItem item)
{
	_items[item->GetSlot()] = item;
}

SharedItem& Inventory::Get(int32 slot)
{
	return _items[slot];
}
optional<int32> Inventory::GetEmptySlot()
{
	for (int32 slot = 0; slot < kSlotCount; ++slot)
	{
		auto iter = find_if(_items.begin(), _items.end(), [&slot](const auto& item) {
			return item.second->GetSlot() == slot;
			});

		if (iter == _items.end())
			return slot;
	}

	return nullopt;
}
