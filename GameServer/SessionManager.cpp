#include "pch.h"
#include "SessionManager.h"
#include "AccountManager.h"

SessionManager g_session_manager;

void SessionManager::SessionThenAccount()
{
	WRITE_LOCK;
	g_account_manager.Lock();
}

void SessionManager::Lock()
{
	WRITE_LOCK;
}
