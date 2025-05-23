#include "pch.h"
#include "ServerDB.h"

ServerDB::ServerDB()
{
	ASSERT_CRASH(g_db_connection_pool->Connect(100, L"Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\MSSQLLocalDB;Database=ServerDB;Trusted_Connection=Yes;"));
}

void ServerDB::MakeTables(bool create)
{
	DBConnection* db_conn = g_db_connection_pool->Pop();

	// account table
	auto account_table_query = 
		create ? 
		L"															\
		DROP TABLE IF EXISTS accounts;								\
		CREATE TABLE accounts										\
		(															\
			[account_id] INT NOT NULL PRIMARY KEY IDENTITY(1,1),	\
			[account_name] NVARCHAR(50)	NOT NULL UNIQUE				\
		);"
		:
		L"															\
		CREATE TABLE accounts										\
		(															\
			[account_id] INT NOT NULL PRIMARY KEY IDENTITY(1,1),	\
			[account_name] NVARCHAR(50)	NOT NULL UNIQUE				\
		);";		
	ASSERT_CRASH(db_conn->Execute(account_table_query));

	// player table
	auto player_table_query = 
		create ?
		L"															\
		DROP TABLE players;											\
		CREATE TABLE players										\
		(															\
			[player_id] INT NOT NULL PRIMARY KEY IDENTITY(1,1),		\
			[player_name] NVARCHAR(50),								\
			[account_id] INT,										\
			CONSTRAINT FK_ACCOUNTS_PLAYERS FOREIGN KEY (account_id) REFERENCES accounts(account_id) \
		);"
		:
		L"															\
		CREATE TABLE players										\
		(															\
			[player_id] INT NOT NULL PRIMARY KEY IDENTITY(1,1),		\
			[player_name] NVARCHAR(50),								\
			[account_id] INT,										\
			CONSTRAINT FK_ACCOUNTS_PLAYERS FOREIGN KEY (account_id) REFERENCES accounts(account_id) \
		);";
	ASSERT_CRASH(db_conn->Execute(player_table_query));

	auto stat_table_query = 
		create ? 
		L"															\
		DROP TABLE IF EXISTS stats;									\
		CREATE TABLE stats											\
		(															\
			[stat_id] INT NOT NULL PRIMARY KEY IDENTITY(1,1),		\
			[level] INT,											\
			[hp] INT,												\
			[max_hp] INT,											\
			[attack] INT,											\
			[speed] FLOAT,											\
			[total_exp] INT,											\
			[player_id] INT UNIQUE,									\
			CONSTRAINT FK_PLAYERS_STATS FOREIGN KEY(player_id) REFERENCES players(player_id) \
		);"
		:
		L"															\
		CREATE TABLE stats											\
		(															\
			[stat_id] INT NOT NULL PRIMARY KEY IDENTITY(1,1),		\
			[level] INT,											\
			[hp] INT,												\
			[max_hp] INT,											\
			[attack] INT,											\
			[speed] FLOAT,											\
			[total_exp] INT,											\
			[player_id] INT UNIQUE,									\
			CONSTRAINT FK_PLAYERS_STATS FOREIGN KEY(player_id) REFERENCES players(player_id) \
		);";
	ASSERT_CRASH(db_conn->Execute(stat_table_query));

	auto item_table_query = 
		create ?
		L"															\
		DROP TABLE IF EXISTS items;									\
		CREATE TABLE items											\
		(															\
			[item_id] INT NOT NULL PRIMARY KEY IDENTITY(1,1),		\
			[data_sheet_id] INT,									\
			[count]	INT,											\
			[slot]	INT,											\
			[equipped] BIT,											\
			[owner_id] INT,											\
			CONSTRAINT FK_PLAYERS_ITEMS FOREIGN KEY(owner_id) REFERENCES players(player_id)	\
		);"
		:
		L"															\
		CREATE TABLE items											\
		(															\
			[item_id] INT NOT NULL PRIMARY KEY IDENTITY(1,1),		\
			[data_sheet_id] INT,									\
			[count]	INT,											\
			[slot]	INT,											\
			[equipped] BIT,											\
			[owner_id] INT,											\
			CONSTRAINT FK_PLAYERS_ITEMS FOREIGN KEY(owner_id) REFERENCES players(player_id)	\
		);";
	ASSERT_CRASH(db_conn->Execute(item_table_query));

	g_db_connection_pool->Push(db_conn);
}
