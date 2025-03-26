#pragma once
#include "GameObject.h"
#include "Inventory.h"

class Player : public GameObject
{
public:
	Player();

public:
	uint64 PlayerDbId = 0;
	string name;
	// caution: cycle
	SharedGameSession OwnerSession;	

	Inventory& GetInventory() { return _inventory; }
	void SetRoom(SharedRoom room) { _room = room; }
	void RefreshStat();	

	SharedViewCube& GetView() { return _view_cube; }

private:
	Inventory _inventory;
	weak_ptr<class Room> _room;
	SharedViewCube _view_cube;
};