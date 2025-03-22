#pragma once
#include "Item.h"

class Inventory
{
public:
	xmap<int16, SharedItem>& Items() { return _items; }

	void Add(const Item& item);
	SharedItem Get(int32 itemDbId);
	SharedItem Find(function<SharedItem(bool)> predicate);
	int32 GetEmptySlot();

private:
	xmap<int16, SharedItem> _items;
};

