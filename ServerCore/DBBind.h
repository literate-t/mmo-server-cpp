#pragma once
#include "DBConnection.h"

// 비트필드를 검증하는 헬퍼
template<int32 C>
struct BitsCheck { enum { value = (1 << (C - 1)) | BitsCheck<C-1>::value}; };

template<>
struct BitsCheck<1> { enum { value = 1 }; };

template<>
struct BitsCheck<0> { enum { value = 0 }; };

template<int32 ParamCount, int32 ColumnCount>
class DBBind
{
public:
	DBBind(DBConnection& db_connnection, const wchar_t* query)
		: _db_connection(db_connnection), _query(query)
	{
		memset(_param_index, 0, sizeof(_param_index));
		memset(_column_index, 0, sizeof(_column_index));
		_param_flag= 0;
		_column_flag = 0;
		_db_connection.Unbind();
	}

	bool Validate()
	{
		return _param_flag == BitsCheck<ParamCount>::value && _column_flag == BitsCheck<ColumnCount>::value;
	}

	bool Execute()
	{
		ASSERT_CRASH(Validate());
		return _db_connection.Execute(_query);
	}

	bool Fetch()
	{
		return _db_connection.Fetch();
	}

public:
	template<typename T>
	void BindParam(int32 index/*0 start*/, T& value)
	{
		_db_connection.BindParam(index + 1, &value, &_param_index[index]);
		_param_flag |= (1LL << index);
	}

	void BindParam(int32 index/*0 start*/, const wchar_t* value)
	{
		_db_connection.BindParam(index + 1, value, &_param_index[index]);
		_param_flag |= (1LL << index);
	}

	void BindParam(int32 index/*0 start*/, const char* value)
	{
		// char* to wchar_t*
		size_t converted_chars;
		mbstowcs_s(&converted_chars, nullptr, 0, value, 0);

		// TODO : delete magic number
		wchar_t dest[50];
		mbstowcs_s(&converted_chars, dest, converted_chars, value, _TRUNCATE);

		_db_connection.BindParam(index + 1, dest, &_param_index[index]);
		_param_flag |= (1LL << index);
	}	

	template<typename T, int32 N>
	void BindParam(int32 index/*0 start*/, T(&value)[N])
	{
		_db_connection.BindParam(index + 1, (const BYTE*)value, sizeof(value), &_param_index[index]);
		_param_flag |= (1LL << index);
	}

	template<typename T>
	void BindParam(int32 index, T* value, int32 N)
	{
		_db_connection.BindParam(index + 1, (const T*)value, sizeof(value), &_param_index[index]);
		_param_flag |= (1LL << index);
	}

	template<typename T>
	void BindColumn(int32 index, T& value)
	{
		_db_connection.BindColumn(index + 1, &value, &_column_index[index]);
		_column_flag |= (1LL << index);
	}

	template<int32 N>
	void BindColumn(int32 index, wchar_t(&value)[N])
	{
		_db_connection.BindColumn(index + 1, value, N , &_column_index[index]);
		_column_flag |= (1LL << index);
	}

	void BindColumn(int32 index, wchar_t* value, int32 len)
	{
		_db_connection.BindColumn(index + 1, value, len, &_column_index[index]);
		_column_flag |= (1LL << index);
	}

	void BindColumn(int32 index, char* value, int32 len)
	{
		_db_connection.BindColumn(index + 1, value, len, &_column_index[index]);
		_column_flag |= (1LL << index);		
	}

	template<typename T, int32 N>
	void BindColumn(int32 index, T(&value)[N])
	{
		_db_connection.BindColumn(index + 1, value, sizeof(value), &_column_index[index]);
		_column_flag |= (1LL << index);
	}

	bool GetId(OUT int32& id)
	{
		return _db_connection.GetData(id);
	}

protected:
	DBConnection& _db_connection;
	const wchar_t* _query;
	SQLLEN _param_index[ParamCount > 0 ? ParamCount : 1];
	SQLLEN _column_index[ColumnCount > 0 ? ColumnCount : 1];
	uint64 _param_flag;
	uint64 _column_flag;
};

