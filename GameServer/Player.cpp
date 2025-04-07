#include "pch.h"
#include "Player.h"
#include "ViewCube.h"
#include "DBSerializer.h"

Player::Player()	
	:GameObject(GameObjectType::PLAYER),
	_weapon_damage(0),
	_armor_defence(0)

{
}

Player::~Player()
{
	cout << "~Player()" << endl;
}

void Player::RefreshStat()
{
	_weapon_damage = 0;
	_armor_defence = 0;

	for (auto& [key, value] : _inventory.GetItems())
	{
		if (value->GetEquipped())
			continue;

		switch (value->GetItemType())
		{
		case ItemType::ITEM_TYPE_WEAPON:
		{
			SharedWeapon weapon = static_pointer_cast<Weapon>(value);
			_weapon_damage += weapon->GetDamage();
			break;
		}
		case ItemType::ITEM_TYPE_ARMOR:
			SharedArmor armor = static_pointer_cast<Armor>(value);
			_armor_defence += armor->GetDefence();
			break;
		}
	}
}

void Player::MakeViewCube()
{
	_view_cube = MakeShared<ViewCube>(static_pointer_cast<Player>(shared_from_this()));
}

void Player::ReleaseViewCube()
{
	_view_cube = nullptr;
}

int32 Player::GetTotalAttack()
{
	return _weapon_damage;
}

int32 Player::GetTotalDefence()
{
	return _armor_defence;
}

void Player::OnLeaveGame()
{
	SharedPlayer player = static_pointer_cast<Player>(shared_from_this());
	if (player == nullptr)
		return;

	DBSerializer::SavePlayerStatusAll(player);
}
