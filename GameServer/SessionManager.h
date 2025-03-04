#pragma once
class SessionManager
{
	USE_LOCK;

public:
	void SessionThenAccount();
	void Lock();
};

extern SessionManager g_session_manager;