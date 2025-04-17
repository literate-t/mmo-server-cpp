#pragma once
#include "GameObject.h"
#include "Inventory.h"

class Player : public GameObject
{
public:
	Player();
	Player(bool is_dummy);
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

	int32 GetTotalAttack() override;
	int32 GetTotalDefence() override;
	bool IsDummy() const;

	// room
	void OnLeaveGame();

	// packet
	void HandleEquipItemPacket(const C_EquipItem& pkt);

protected:
	bool _is_dummy;

private:
	Inventory _inventory;
	SharedViewCube _view_cube;

	int32 _weapon_damage;
	int32 _armor_defence;
};