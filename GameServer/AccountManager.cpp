#include "pch.h"
#include "AccountManager.h"
#include "PlayerManager.h"

AccountManager g_account_manager;

void AccountManager::AccountThenPlayer()
{
	WRITE_LOCK;
	// ��� sleep_for() ���� ������ ����� ��Ȳ �̹߻�
	//this_thread::sleep_for(100ms);
	g_player_manager.Lock();
}

void AccountManager::Lock()
{
	WRITE_LOCK;
}