#include "pch.h"
#include "ServerSessionManager.h"
#include "ServerSession.h"

int32 ServerSessionManager::GenerateDummyId()
{
    WRITE_LOCK;
    int32 id = Instance()->_dummy_session_id;
    Instance()->_dummy_session_id = ++id;

    return id;
}

shared_ptr<ServerSessionManager> ServerSessionManager::Instance()
{
    static shared_ptr<ServerSessionManager> _instance = make_shared<ServerSessionManager>();
    return _instance;
}
