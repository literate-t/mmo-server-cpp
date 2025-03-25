#pragma once
#include "GameObject.h"
#include "Inventory.h"

class Player : public GameObject
{
public:
	uint64 PlayerDbId = 0;
	string name;
	// caution: cycle
	SharedGameSession OwnerSession;

	Inventory& GetInventory() { return _inventory; }
private:
	Inventory _inventory;
};