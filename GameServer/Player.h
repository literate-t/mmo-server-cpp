#pragma once
#include "GameObject.h"
#include "Inventory.h"

class Player : public GameObject
{
public:
	Player();
	~Player();

public:
	uint64 PlayerDbId = 0;
	// caution: cycle
	SharedGameSession OwnerSession;	

	Inventory& GetInventory() { return _inventory; }	
	void RefreshStat();	

	SharedViewCube& GetView() { return _view_cube; }
	void MakeViewCube();
	void ReleaseViewCube();

	int32 GetWeaponDamage() const { return _weapon_damage; }
	int32 GetArmorDamage() const { return _armor_defence; }

private:
	Inventory _inventory;
	SharedViewCube _view_cube;

	int32 _weapon_damage;
	int32 _armor_defence;
};