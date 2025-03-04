#include "pch.h"
#include "AccountManager.h"
#include "PlayerManager.h"

AccountManager g_account_manager;

void AccountManager::AccountThenPlayer()
{
	WRITE_LOCK;
	// 잠깐 sleep_for() 하지 않으면 데드락 상황 미발생
	//this_thread::sleep_for(100ms);
	g_player_manager.Lock();
}

void AccountManager::Lock()
{
	WRITE_LOCK;
}