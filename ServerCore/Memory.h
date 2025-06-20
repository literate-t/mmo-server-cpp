#pragma once

// ------- xnew ------- //
template<typename Type, typename ...Args>
Type* xnew(Args&& ...args)
{
	Type* malloc_memory;
#if defined(_STOMP)
	malloc_memory = StompAllocator::Allocate(sizeof(Type));
#elif defined(_SIZE_POOL)
	malloc_memory = static_cast<Type*>(PoolAllocator::Allocate(sizeof(Type)));	
#endif

	// init object with ctor
	new (malloc_memory)Type(std::forward<Args>(args)...);

	return malloc_memory;
}

// ------- xdelete ------- //
template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type();
#if defined(_STOMP)
	StompAllocator::Release(sizeof(Type));
#elif defined(_SIZE_POOL)
	PoolAllocator::Release(obj);
#endif
}

// ------- MakeShared ------- //
template<typename Type, typename...Args>
shared_ptr<Type> MakeShared(Args&& ...args)
{
	return shared_ptr<Type>{xnew<Type>(forward<Args>(args)...), xdelete<Type>};
}