#include "pch.h"
#include "Player.h"
#include "ViewCube.h"
#include "DBSerializer.h"
#include "GameSession.h"
#include "ClientPacketHandler.h"

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
		if (!value->GetEquipped())
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

	_weapon_damage += GetStatInfo().attack();	
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

	DBSerializer::SavePlayerStatusNoti(player);
}

void Player::HandleEquipItemPacket(const C_EquipItem& equip_item)
{	
	SharedItem find_item = GetInventory().Get(equip_item.slot());
	if (find_item == nullptr)
		return;

	if (find_item->GetItemType() == ItemType::ITEM_TYPE_CONSUMABLE)
		return;

	SharedPlayer this_player = static_pointer_cast<Player>(shared_from_this());

	if (find_item->GetItemType() == ItemType::ITEM_TYPE_WEAPON)
	{
		// Player is archer
		SharedWeapon weapon = static_pointer_cast<Weapon>(find_item);
		if (weapon->GetWeaponType() != WEAPON_TYPE_BOW)
			return;
	}

	// Unequip same type item equipped
	if (equip_item.equipped())
	{
		SharedItem to_unequip_item;
		if (find_item->GetItemType() == ItemType::ITEM_TYPE_WEAPON)
		{
			to_unequip_item = GetInventory().FindIf([](const SharedItem& item){ 
				return item->GetEquipped() && item->GetItemType() == ItemType::ITEM_TYPE_WEAPON;
				});
		}
		else if (find_item->GetItemType() == ItemType::ITEM_TYPE_ARMOR)
		{
			to_unequip_item = GetInventory().FindIf([&find_item](const SharedItem& item) {
				SharedWeapon find_weapon = static_pointer_cast<Weapon>(find_item);
				SharedWeapon weapon = static_pointer_cast<Weapon>(item);

				return item->GetEquipped() && item->GetItemType() == ItemType::ITEM_TYPE_ARMOR && weapon->GetWeaponType() == find_weapon->GetWeaponType();
				});
		}

		// Unequip before item response
		if (to_unequip_item)
		{
			to_unequip_item->SetEquipped(false);

			// DB			
			DBSerializer::EquipItemNoti(this_player, to_unequip_item);

			// Client
			S_EquipItem s_equip;
			s_equip.set_equipped(to_unequip_item->GetEquipped());
			s_equip.set_slot(to_unequip_item->GetSlot());
			OwnerSession->Send(ClientPacketHandler::MakeSendBuffer(s_equip));
		}
	}

	// Equip
	find_item->SetEquipped(equip_item.equipped());
	DBSerializer::EquipItemNoti(this_player, find_item);

	S_EquipItem s_equip;
	s_equip.set_equipped(equip_item.equipped());
	s_equip.set_slot(equip_item.slot());
	OwnerSession->Send(ClientPacketHandler::MakeSendBuffer(s_equip));

	RefreshStat();
}

void Player::HandleUseItemPacket(const C_UseItem& pkt)
{
	SharedItem find_item = GetInventory().Get(pkt.slot());
	if (find_item == nullptr)
		return;

	if (find_item->GetItemType() != ItemType::ITEM_TYPE_CONSUMABLE)
		return;

	if (GetHp() == GetMaxHp())
		return;

	SharedConsumbale consumable = static_pointer_cast<Consumable>(find_item);
	AddHp(consumable->GetDamage());

	if (0 == GetInventory().Erase(pkt.slot()))
		return;

	SharedPlayer this_player = static_pointer_cast<Player>(shared_from_this());
	DBSerializer::DropItemNoti(this_player, find_item);

	S_ChangeHp change_hp;
	change_hp.set_hp(GetHp());
	change_hp.set_objectid(GetObjectId());
	OwnerSession->Send(ClientPacketHandler::MakeSendBuffer(change_hp));

	S_UseItem use_item;
	use_item.set_slot(pkt.slot());
	OwnerSession->Send(ClientPacketHandler::MakeSendBuffer(use_item));
}

void Player::HandleDropItemPacket(const C_DropItem& pkt)
{
	SharedItem find_item = GetInventory().Get(pkt.slot());
	if (find_item == nullptr)
		return;

	if (0 == GetInventory().Erase(pkt.slot()))
		return;

	SharedPlayer this_player = static_pointer_cast<Player>(shared_from_this());
	DBSerializer::DropItemNoti(this_player, find_item);

	S_DropItem drop_item;
	drop_item.set_slot(pkt.slot());
	OwnerSession->Send(ClientPacketHandler::MakeSendBuffer(drop_item));
}
