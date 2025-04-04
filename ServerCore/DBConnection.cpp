#include "pch.h"
#include "DBConnection.h"

bool DBConnection::Connect(SQLHENV henv, const WCHAR* connection_string)
{
	if (SQLAllocHandle(SQL_HANDLE_DBC, henv, &_connection) != SQL_SUCCESS)
		return false;

	WCHAR conn_string_buffer[MAX_PATH] = {};
	wcscpy_s(conn_string_buffer, connection_string);

	WCHAR result_string[MAX_PATH] = {};
	SQLSMALLINT result_string_len = 0;

	SQLRETURN sql_return = SQLDriverConnectW(
		_connection,
		nullptr,
		reinterpret_cast<SQLWCHAR*>(conn_string_buffer),
		_countof(conn_string_buffer),
		OUT reinterpret_cast<SQLWCHAR*>(result_string),
		_countof(result_string),
		OUT & result_string_len,
		SQL_DRIVER_NOPROMPT
	);

	if (SQLAllocHandle(SQL_HANDLE_STMT, _connection, &_statement) != SQL_SUCCESS)
		return false;

	return sql_return == SQL_SUCCESS || sql_return == SQL_SUCCESS_WITH_INFO;
}

void DBConnection::Clear()
{
	if (_connection != SQL_NULL_HANDLE)
	{
		SQLFreeHandle(SQL_HANDLE_DBC, _connection);
		_connection = SQL_NULL_HANDLE;
	}

	if (_statement != SQL_NULL_HANDLE)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, _statement);
		_connection = SQL_NULL_HANDLE;
	}
}

bool DBConnection::Execute(const TCHAR* query)
{
	SQLRETURN sql_return = SQLExecDirect(_statement, (SQLTCHAR*)query, SQL_NTSL);
	if (sql_return == SQL_SUCCESS || sql_return == SQL_SUCCESS_WITH_INFO)
		return true;

	HandleError(sql_return);

	return false;
}

bool DBConnection::Fetch()
{
	SQLRETURN sql_return = SQLFetch(_statement);

	switch (sql_return)
	{
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:
		return true;
	case SQL_NO_DATA:
		return false;
	case SQL_ERROR:
		HandleError(sql_return);
		return false;
	default:
		return true;
	}
}

int32 DBConnection::GetRowCount()
{
	SQLLEN count = 0;
	SQLRETURN sql_return = SQLRowCount(_statement, OUT & count);

	if (sql_return == SQL_SUCCESS || sql_return == SQL_SUCCESS_WITH_INFO)
		return static_cast<int32>(count);

	return int32();
}

// 기존에 바인드된 값들을 초기화
void DBConnection::Unbind()
{
	SQLFreeStmt(_statement, SQL_UNBIND);
	SQLFreeStmt(_statement, SQL_RESET_PARAMS);
	SQLFreeStmt(_statement, SQL_CLOSE);
}

bool DBConnection::BindParam(int32 param_index, bool* value, SQLLEN* index)
{
	return BindParam(param_index, SQL_C_TINYINT, SQL_TINYINT, sizeof32(bool), value, index);
}

bool DBConnection::BindParam(int32 param_index, float* value, SQLLEN* index)
{
	// 일반 정수가 아닌 경우 sizeof(float)이 아니라 0으로 해도 된다고 하는데 일단 이렇게 둔다
	return BindParam(param_index, SQL_C_FLOAT, SQL_REAL, sizeof(float), value, index);
}

bool DBConnection::BindParam(int32 param_index, double* value, SQLLEN* index)
{
	return BindParam(param_index, SQL_C_DOUBLE, SQL_DOUBLE, sizeof32(double), value, index);
}

bool DBConnection::BindParam(int32 param_index, int8* value, SQLLEN* index)
{
	return BindParam(param_index, SQL_C_STINYINT, SQL_CHAR, sizeof(int8), value, index);
}

bool DBConnection::BindParam(int32 param_index, int16* value, SQLLEN* index)
{
	return BindParam(param_index, SQL_C_SSHORT, SQL_SMALLINT, sizeof(int16), value, index);
}

bool DBConnection::BindParam(int32 param_index, int32* value, SQLLEN* index)
{
	return BindParam(param_index, SQL_C_SLONG, SQL_INTEGER, sizeof(int32), value, index);
}

bool DBConnection::BindParam(int32 param_index, int64* value, SQLLEN* index)
{
	return BindParam(param_index, SQL_C_SBIGINT, SQL_BIGINT, sizeof(int64), value, index);
}

bool DBConnection::BindParam(int32 param_index, uint64* value, SQLLEN* index)
{
	return BindParam(param_index, SQL_C_UBIGINT, SQL_BIGINT, sizeof(uint64), value, index);
}

bool DBConnection::BindParam(int32 param_index, TIMESTAMP_STRUCT* value, SQLLEN* index)
{
	return BindParam(param_index, SQL_C_TYPE_TIMESTAMP, SQL_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), value, index);
}

bool DBConnection::BindParam(int32 param_index, const TCHAR* string, SQLLEN* index)
{
	SQLLEN size = (wcslen(string) + 1) * 2;
	*index = SQL_NTSL;

	if (size > WVARCHAR_MAX)
		return BindParam(param_index, SQL_C_TCHAR, SQL_WLONGVARCHAR, size, (SQLPOINTER)string, index);
	else
		return BindParam(param_index, SQL_C_TCHAR, SQL_WVARCHAR, size, (SQLPOINTER)string, index);
}

bool DBConnection::BindParam(int32 param_index, const BYTE* bin, int32 size, SQLLEN* index)
{
	if (bin == nullptr)
	{
		*index = SQL_NULL_DATA;
		size = 1;
	}
	else
		*index = size;

	if (size > BINARY_MAX)
		return BindParam(param_index, SQL_C_BINARY, SQL_LONGVARBINARY, size, (BYTE*)bin, index);
	else
		return BindParam(param_index, SQL_C_BINARY, SQL_BINARY, size, (BYTE*)bin, index);
}

bool DBConnection::BindColumn(int32 column_index, bool* value, SQLLEN* index)
{
	return BindColumn(column_index, SQL_C_TINYINT, sizeof(bool), value, index);
}

bool DBConnection::BindColumn(int32 column_index, float* value, SQLLEN* index)
{
	return BindColumn(column_index, SQL_C_FLOAT, sizeof(float), value, index);;
}

bool DBConnection::BindColumn(int32 column_index, double* value, SQLLEN* index)
{
	return BindColumn(column_index, SQL_C_DOUBLE, sizeof(double), value, index);;
}

bool DBConnection::BindColumn(int32 column_index, int8* value, SQLLEN* index)
{
	return BindColumn(column_index, SQL_C_STINYINT, sizeof(int8), value, index);;
}

bool DBConnection::BindColumn(int32 column_index, int16* value, SQLLEN* index)
{
	return BindColumn(column_index, SQL_C_SSHORT, sizeof(int16), value, index);
}

bool DBConnection::BindColumn(int32 column_index, int32* value, SQLLEN* index)
{
	return BindColumn(column_index, SQL_C_SLONG, sizeof(int32), value, index);
}

bool DBConnection::BindColumn(int32 column_index, int64* value, SQLLEN* index)
{
	return BindColumn(column_index, SQL_C_SBIGINT, sizeof(int64), value, index);
}

bool DBConnection::BindColumn(int32 column_index, TIMESTAMP_STRUCT* value, SQLLEN* index)
{
	return BindColumn(column_index, SQL_C_TYPE_TIMESTAMP, sizeof(TIMESTAMP_STRUCT), value, index);
}

bool DBConnection::BindColumn(int32 column_index, TCHAR* value, int32 size, SQLLEN* index)
{
	return BindColumn(column_index, SQL_C_TCHAR, size, value, index);
}

bool DBConnection::BindColumn(int32 column_index, CHAR* value, int32 size, SQLLEN* index)
{
	return BindColumn(column_index, SQL_C_CHAR, size, value, index);
}

bool DBConnection::BindColumn(int32 column_index, BYTE* value, int32 size, SQLLEN* index)
{
	return BindColumn(column_index, SQL_BINARY, size, value, index);
}

bool DBConnection::GetData(OUT int32& id)
{
	SQLRETURN ret = SQLMoreResults(_statement);
	if (SQL_SUCCEEDED(ret))
	{
		ret = SQLFetch(_statement);
		if (SQL_SUCCEEDED(ret))
		{
			ret = SQLGetData(_statement, 1, SQL_C_SLONG, &id, 0, nullptr);
			if (SQL_SUCCEEDED(ret))
				return true;
			else
				return false;
		}
	}
	return false;
}

bool DBConnection::BindParam(SQLUSMALLINT parameter_index, SQLSMALLINT c_type, SQLSMALLINT sql_type, SQLLEN len, SQLPOINTER ptr, SQLLEN* index)
{
	SQLRETURN sql_return = SQLBindParameter(_statement, parameter_index, SQL_PARAM_INPUT, c_type, sql_type, len, 0, ptr, 0, index);
	if (sql_return != SQL_SUCCESS && sql_return != SQL_SUCCESS_WITH_INFO)
	{
		HandleError(sql_return);
		return false;
	}

	return true;
}

bool DBConnection::BindColumn(SQLUSMALLINT column_index, SQLSMALLINT target_type, SQLULEN len, SQLPOINTER value, SQLLEN* index)
{
	SQLRETURN sql_return = SQLBindCol(_statement, column_index, target_type, value, len, index);
	if (sql_return != SQL_SUCCESS && sql_return != SQL_SUCCESS_WITH_INFO)
	{
		HandleError(sql_return);
		return false;
	}

	return true;
}

void DBConnection::HandleError(SQLRETURN sql_return)
{
	if (sql_return == SQL_SUCCESS)
		return;

	SQLSMALLINT index = 1;
	SQLWCHAR sql_state[MAX_PATH] = {};
	SQLINTEGER native_error = 0;
	SQLWCHAR error_message[MAX_PATH] = {};
	SQLSMALLINT message_length = 0;
	SQLRETURN error_return = 0;

	while (true)
	{
		error_return = SQLGetDiagRecW(
			SQL_HANDLE_STMT,
			_statement,
			index,
			OUT sql_state,
			OUT & native_error,
			OUT error_message,
			_countof(error_message),
			OUT & message_length
		);

		if (error_return == SQL_NO_DATA)
			break;

		// SQLGetDiagRecW() 정상 동작하지 않았다는 것
		if (error_return != SQL_SUCCESS && error_return != SQL_SUCCESS_WITH_INFO)
			break;

		// TODO : Log
		wcout.imbue(locale("kor"));
		wcout << error_message << endl;

		++index;
	}
}
