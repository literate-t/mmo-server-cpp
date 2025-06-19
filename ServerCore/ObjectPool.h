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
#if defined(_STOMP)
		Type* memory = static_cast<Type*>(StompAllocator::Allocate(s_alloc_size));
#elif defined(_SIZE_POOL)
		Type* memory = static_cast<Type*>(MemoryHeader::Initialize(s_pool.Pop(), s_alloc_size));
#endif
		new(memory)Type(forward<Args>(args)...);

		return memory;
	}

	static void Push(Type* obj)
	{
		obj->~Type();
#if defined(_STOMP)
		StompAllocator::Release(obj);
#elif defined(_SIZE_POOL)
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