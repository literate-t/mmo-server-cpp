#pragma once
class ServerSessionManager
{
public:
	int32 GenerateDummyId();
	static shared_ptr<ServerSessionManager> Instance();

private:
	int32 _dummy_session_id = 0;

	USE_LOCK;
};

