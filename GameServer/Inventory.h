#pragma once
#include "Item.h"

class Inventory
{
public:
	xmap<int16, SharedItem>& GetItems() { return _items; }

	void Add(SharedItem item);
	SharedItem& Get(int32 slot);
	SharedItem FindIf(function<bool(const SharedItem&)> predicate);
	optional<int32> GetEmptySlot();

private:
	xmap<int16, SharedItem> _items;
	static constexpr int32 kSlotCount = 20;
};

