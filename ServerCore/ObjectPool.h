#pragma once

#include "Types.h"
#include "MemoryPool.h"

// choose between StompAllocator and ObjectPool

template<typename Type>
class ObjectPool
{
public:
	template<typename... Args>
	static Type* Pop(Args&&... args)
	{
#ifdef _STOMP
		MemoryHeader* ptr = reinterpret_cast<MemoryHeader*>(StompAllocator::Allocate(s_alloc_size));
		Type* memory = reinterpret_cast<Type*>(ptr);
#else
		Type* memory = static_cast<Type*>(MemoryHeader::Initialize(s_pool.Pop(), s_alloc_size));
#endif
		new(memory)Type(forward<Args>(args)...);

		return memory;
	}

	static void Push(Type* obj)
	{
		obj->~Type();
#ifdef _STOMP
		StompAllocator::Release(obj);
#else
		s_pool.Push(MemoryHeader::Release(obj));
#endif
	}

	template<typename...Args>
	static shared_ptr<Type> MakeShared(Args&&...args)
	{
		shared_ptr<Type> sptr{ Pop(forward<Args>(args)...), Push };

		return sptr;
	}

private:
	// Type 별로 존재하게 된다
	static int32		s_alloc_size;
	static MemoryPool	s_pool;
};

template<typename Type>
int32 ObjectPool<Type>::s_alloc_size{ sizeof(Type) + sizeof(MemoryHeader) };

template<typename Type>
MemoryPool ObjectPool<Type>::s_pool{ s_alloc_size };