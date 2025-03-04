#pragma once
#include "DBConnection.h"

class DBConnectionPool
{
public:
	DBConnectionPool();
	~DBConnectionPool();

	bool Connect(int32 connection_count, const WCHAR* connection_string);
	void Clear();

	DBConnection* Pop();
	void Push(DBConnection* connection);

private:
	USE_LOCK;
	SQLHENV _environment = SQL_NULL_HANDLE;
	xvector<DBConnection*> _connections;
};

