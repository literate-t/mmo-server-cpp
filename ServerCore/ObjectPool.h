#pragma once

#include "Types.h"
#include "SlabMemoryManager.h"
#include "MemoryPool.h"

template<typename Type>
class ObjectPool
{
public:
	// ------- MakeShared ------- //
	template<typename...Args>
	static shared_ptr<Type> MakeShared(Args&&...args)
	{
		shared_ptr<Type> sptr{ Pop(forward<Args>(args)...), Push };

		return sptr;
	}

	// ------- MakeSharedThreadSlab ------- //
	template<typename...Args>
	static shared_ptr<Type> MakeSharedThreadSlab(Args&&...args)
	{
		shared_ptr<Type> sptr{ PopThreadSlab(forward<Args>(args)...), PushThreadSlab };

		return sptr;
	}

	// ------- Pop ------- //
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

	// ------- PopThreadSlab ------- //
	template<typename... Args>
	static Type* PopThreadSlab(Args&&... args)
	{
		Type* memory = static_cast<Type*>(tls_pool_alloc(s_alloc_size));
		new(memory)Type(forward<Args>(args)...);

		return memory;
	}

	// ------- Push ------- //
	static void Push(Type* obj)
	{
		obj->~Type();
#if defined(_STOMP)
		StompAllocator::Release(obj);
#elif defined(_SIZE_POOL)
		s_pool.Push(MemoryHeader::DetachHeader(obj));
#endif
	}

	// ------- PushThreadSlab ------- //
	static void PushThreadSlab(Type* obj)
	{
		obj->~Type();
		tls_pool_free(obj);
	}

private:
	// Type 별로 존재하게 된다
	static int32		s_alloc_size;

#if defined(_TLS_SLAB)
#elif defined(_SIZE_POOL)
	static MemoryPool	s_pool;
#endif
};

template<typename Type>
int32 ObjectPool<Type>::s_alloc_size{ sizeof(Type) + sizeof(MemoryHeader) };

#if defined(_TLS_SLAB)
#elif defined(_SIZE_POOL)
template<typename Type>
MemoryPool ObjectPool<Type>::s_pool{ s_alloc_size };
#endif