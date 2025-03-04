#pragma once

#include "Types.h"

#pragma region TypeList
template<typename...T>
struct TypeList;

// 타입을 두 개 넣은 TypeList를 사용할 때
// 밑에 가변인자를 받는 템플릿이 있지만 이 템플릿이 더 가까움
template<typename T, typename U>
struct TypeList<T, U>
{
	using Head = T;
	using Tail = U;
};

template<typename T, typename...U>
struct TypeList<T, U...>
{
	using Head = T;
	using Tail = TypeList<U...>;
};
#pragma endregion

#pragma region Length
template<typename T>
struct Length;

template<>
struct Length<TypeList<>>
{
	enum { value = 0 };
};

template<typename T, typename... U>
struct Length<TypeList<T, U...>>
{
	enum { value = 1 + Length<TypeList<U...>>::value };
};
#pragma endregion

#pragma region TypeAt
template<typename TypeList, int32 index>
struct TypeAt;

template<typename Head, typename...Tail>
struct TypeAt<TypeList<Head, Tail...>, 0>
{
	using Result = Head;
};

template<typename Head, typename...Tail, int32 index>
struct TypeAt<TypeList<Head, Tail...>, index>
{
	using Result = typename TypeAt<TypeList<Tail...>, index - 1>::Result;
};
#pragma endregion

#pragma region IndexOf
template<typename TL, typename Type>
struct IndexOf;

template<typename...Tail, typename Type>
struct IndexOf<TypeList<Type, Tail...>, Type>
{
	enum { value = 0 };
};

template<typename Type>
struct IndexOf<TypeList<>, Type>
{
	enum { value = -1 };
};

template<typename Head, typename...Tail, typename Type>
struct IndexOf<TypeList<Head, Tail...>, Type>
{
private:
	enum { temp = IndexOf<TypeList<Tail...>, Type>::value };

public:
	enum { value = temp == -1 ? -1 : temp + 1 };
};
#pragma endregion

#pragma region Conversion
template<typename From, typename To>
class Conversion
{
private:
	using Small = __int8;
	using Big	= __int32;

	static Small Test(const To&) { return 0; }
	static Big Test(...) { return 0; }
	static From MakeFrom() { return 0; }

public:
	enum
	{
		exists = sizeof(Test(MakeFrom())) == sizeof(Small)
	};
};

#pragma endregion

#pragma region TypeCast
template<int32 T>
struct IntToType
{
	enum { value = T };
};

template<typename TL>
class TypeConversion
{
public:
	enum
	{
		length = Length<TL>::value
	};

	TypeConversion()
	{
		MakeTable(IntToType<0>(), IntToType<0>());
	}

	template<int32 I, int32 J>
	static void MakeTable(IntToType<I>, IntToType<J>)
	{
		using FromType = typename TypeAt<TL, I>::Result;
		using ToType = typename TypeAt<TL, J>::Result;

		if (Conversion<const FromType, const ToType>::exists)
			s_convert[I][J] = true;
		else
			s_convert[I][J] = false;

		MakeTable(IntToType<I>(), IntToType<J + 1>());
	}

	template<int32 I>
	static void MakeTable(IntToType<I>, IntToType<length>)
	{
		MakeTable(IntToType<I + 1>(), IntToType<0>());
	}

	template<int32 J>
	static void MakeTable(IntToType<length>, IntToType<J>)
	{		
	}

	static inline bool IsConvertable(int32 from, int32 to)
	{
		static TypeConversion conversion;

		return conversion.s_convert[from][to];
	}

public:
	// enum 값은 컴파일 타임에 결정된다
	static bool s_convert[length][length];
};

template<typename TL>
bool TypeConversion<TL>::s_convert[length][length] = {};

template<typename To, typename From>
To TypeCast(From* ptr)
{
	if (nullptr == ptr)
		return nullptr;

	using TL = typename From::TL;

	if (TypeConversion<TL>::IsConvertable(ptr->_type_id, IndexOf<TL, remove_pointer_t<To>>::value))
		return static_cast<To>(ptr);

	return nullptr;
}

template<typename To, typename From>
shared_ptr<To> TypeCast(shared_ptr<From> ptr)
{
	if (nullptr == ptr)
		return nullptr;

	using TL = typename From::TL;

	if (TypeConversion<TL>::IsConvertable(ptr->_type_id, IndexOf<TL, remove_pointer_t<To>>::value))
		return static_pointer_cast<To>(ptr);

	return nullptr;
}

template<typename To, typename From>
bool CanCast(From* ptr)
{
	if (nullptr == ptr)
		return false;

	using TL = typename From::TL;

	return TypeConversion<TL>::IsConvertable(ptr->_type_id, IndexOf<TL, remove_pointer_t<To>>::value);
}

template<typename To, typename From>
bool CanCast(shared_ptr<From> ptr)
{
	if (nullptr == ptr)
		return false;

	using TL = typename From::TL;

	return TypeConversion<TL>::IsConvertable(ptr->_type_id, IndexOf<TL, remove_pointer_t<To>>::value);
}
#pragma endregion

#define DECLARE_TL		using TL = TL; int32 _type_id
#define INIT_TL(Type)	_type_id = IndexOf<TL, Type>::value