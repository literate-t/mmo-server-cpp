#include "pch.h"
#include "Inventory.h"

void Inventory::Add(SharedItem item)
{
	_items[item->ItemDbId()] = item;
}
