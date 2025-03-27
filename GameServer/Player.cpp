#include "pch.h"
#include "Player.h"
#include "ViewCube.h"

Player::Player()	
{
	_object_type = GameObjectType::PLAYER;
}

void Player::RefreshStat()
{
	_weapon_damage = 0;
	_armor_defence = 0;

	for (auto& [key, value] : _inventory.GetItems())
	{
		if (value->Equipped())
			continue;

		switch (value->ItemType())
		{
		case ItemType::ITEM_TYPE_WEAPON:
		{
			SharedWeapon weapon = static_pointer_cast<Weapon>(value);
			_weapon_damage += weapon->Damage();
			break;
		}
		case ItemType::ITEM_TYPE_ARMOR:
			SharedArmor armor = static_pointer_cast<Armor>(value);
			_armor_defence += armor->Defence();
			break;
		}
	}
}

void Player::MakeViewCube()
{
	_view_cube = MakeShared<ViewCube>(static_pointer_cast<Player>(shared_from_this()));
}
