#include "pch.h"
#include "ServerDB.h"

void ServerDB::Init(bool create)
{
	ASSERT_CRASH(g_db_connection_pool->Connect(10, L"Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\MSSQLLocalDB;Database=ServerDB;Trusted_Connection=Yes;"));
	DBConnection* db_conn = g_db_connection_pool->Pop();

	// account table
	auto account_table_query = 
		create ? 
		L"															\
		DROP TABLE IF EXISTS [dbo].[Account];						\
		CREATE TABLE [dbo].[Account]								\
		(															\
			[AccountId] INT NOT NULL PRIMARY KEY IDENTITY(1,1),	\
			[AccountName] NVARCHAR(50)								\
		);"
		:
		L"															\
		CREATE TABLE [dbo].[Account]								\
		(															\
			[AccountId] INT NOT NULL PRIMARY KEY IDENTITY(1,1),	\
			[AccountName] NVARCHAR(50)								\
		);";		
	ASSERT_CRASH(db_conn->Execute(account_table_query));

	// player table
	auto player_table_query = 
		create ?
		L"															\
		DROP TABLE [dbo].[Player];									\
		CREATE TABLE [dbo].[Player]									\
		(															\
			[PlayerId] INT NOT NULL PRIMARY KEY IDENTITY(1,1),		\
			[PlayerName] NVARCHAR(50),								\
			[AccountId] INT,										\
			CONSTRAINT FK_ACCOUNT_PLAYER FOREIGN KEY (AccountId) REFERENCES Account(AccountId) \
		);"
		:
		L"									\
		CREATE TABLE [dbo].[Player]									\
		(															\
			[PlayerId] INT NOT NULL PRIMARY KEY IDENTITY(1,1),		\
			[PlayerName] NVARCHAR(50),								\
			[AccountId] INT,										\
			CONSTRAINT FK_ACCOUNT_PLAYER FOREIGN KEY (AccountId) REFERENCES Account(AccountId) \
		);";
	ASSERT_CRASH(db_conn->Execute(player_table_query));

	auto stat_table_query = 
		create ? 
		L"															\
		DROP TABLE IF EXISTS [dbo].[Stat];							\
		CREATE TABLE [dbo].[Stat]									\
		(															\
			[StatId] INT NOT NULL PRIMARY KEY IDENTITY(1,1),		\
			[Level] INT,											\
			[Hp] INT,												\
			[MaxHp] INT,											\
			[Attack] INT,											\
			[Speed] INT,											\
			[TotalExp] INT,											\
			[PlayerId] INT UNIQUE,									\
			CONSTRAINT FK_PLAYER_STAT FOREIGN KEY(PlayerId) REFERENCES Player(PlayerId) \
		);"
		:
		L"															\
		CREATE TABLE [dbo].[Stat]									\
		(															\
			[StatId] INT NOT NULL PRIMARY KEY IDENTITY(1,1),		\
			[Level] INT,											\
			[Hp] INT,												\
			[MaxHp] INT,											\
			[Attack] INT,											\
			[Speed] INT,											\
			[TotalExp] INT,											\
			[PlayerId] INT UNIQUE,									\
			CONSTRAINT FK_PLAYER_STAT FOREIGN KEY(PlayerId) REFERENCES Player(PlayerId) \
		);";
	ASSERT_CRASH(db_conn->Execute(stat_table_query));

	auto item_table_query = 
		create ?
		L"															\
		DROP TABLE IF EXISTS [dbo].[Item];							\
		CREATE TABLE [dbo].[Item]									\
		(															\
			[ItemId] INT NOT NULL PRIMARY KEY IDENTITY(1,1),		\
			[DataSheetId] INT,										\
			[Count]	INT,											\
			[Slot]	INT,											\
			[Equipped] BIT,											\
			[OwnerId] INT,											\
			CONSTRAINT FK_PLAYER_ITEM FOREIGN KEY(OwnerId) REFERENCES Player(PlayerId)	\
		);"
		:
		L"															\
		CREATE TABLE [dbo].[Item]									\
		(															\
			[ItemId] INT NOT NULL PRIMARY KEY IDENTITY(1,1),		\
			[DataSheetId] INT,										\
			[Count]	INT,											\
			[Slot]	INT,											\
			[Equipped] BIT,											\
			[OwnerId] INT,											\
			CONSTRAINT FK_PLAYER_ITEM FOREIGN KEY(OwnerId) REFERENCES Player(PlayerId)	\
		);";
	ASSERT_CRASH(db_conn->Execute(item_table_query));

	g_db_connection_pool->Push(db_conn);
}
