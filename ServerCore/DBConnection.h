#pragma once
#include <sql.h>
#include <sqlext.h>

enum
{
	WVARCHAR_MAX = 4000,
	BINARY_MAX = 8000,
};

class DBConnection
{
public:
	bool Connect(SQLHENV henv, const WCHAR* connection_string);
	void Clear();

	bool Execute(const TCHAR* query);
	bool Fetch();
	int32 GetRowCount();
	void Unbind();

public:
	bool BindParam(int32 param_index, bool* value, SQLLEN* index);
	bool BindParam(int32 param_index, float* value, SQLLEN* index);
	bool BindParam(int32 param_index, double* value, SQLLEN* index);
	bool BindParam(int32 param_index, int8* value, SQLLEN* index);
	bool BindParam(int32 param_index, int16* value, SQLLEN* index);
	bool BindParam(int32 param_index, int32* value, SQLLEN* index);
	bool BindParam(int32 param_index, int64* value, SQLLEN* index);
	bool BindParam(int32 param_index, TIMESTAMP_STRUCT* value, SQLLEN* index);
	bool BindParam(int32 param_index, const TCHAR* string, SQLLEN* index);
	bool BindParam(int32 param_index, const BYTE* bin, int32 size, SQLLEN* index);

	bool BindColumn(int32 column_index, bool* value, SQLLEN* index);
	bool BindColumn(int32 column_index, float* value, SQLLEN* index);
	bool BindColumn(int32 column_index, double* value, SQLLEN* index);
	bool BindColumn(int32 column_index, int8* value, SQLLEN* index);
	bool BindColumn(int32 column_index, int16* value, SQLLEN* index);
	bool BindColumn(int32 column_index, int32* value, SQLLEN* index);
	bool BindColumn(int32 column_index, int64* value, SQLLEN* index);
	bool BindColumn(int32 column_index, TIMESTAMP_STRUCT* value, SQLLEN* index);
	bool BindColumn(int32 column_index, TCHAR* value, int32 size, SQLLEN* index);
	bool BindColumn(int32 column_index, CHAR* value, int32 size, SQLLEN* index);
	bool BindColumn(int32 column_index, BYTE* value, int32 size, SQLLEN* index);

	bool GetData(OUT int32& id);

private:
	bool BindParam(SQLUSMALLINT parameter_index, SQLSMALLINT c_type, SQLSMALLINT sql_type, SQLLEN len, SQLPOINTER ptr, SQLLEN* index);
	bool BindColumn(SQLUSMALLINT column_index, SQLSMALLINT target_type, SQLULEN len, SQLPOINTER value, SQLLEN* index);
	void HandleError(SQLRETURN sql_return);

private:
	SQLHDBC _connection = SQL_NULL_HANDLE;
	SQLHSTMT _statement = SQL_NULL_HANDLE;
};

