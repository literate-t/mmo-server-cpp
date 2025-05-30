#pragma once

class MemoryPool;
class MemoryManager
{
	enum
	{
		// ~1024까지는 32단위, ~2048까지는 128단위, ~4096까지 256단위로 증가
		// 단위가 작을수록 촘촘하게
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		MAX_ALLOC_SIZE = 4096
	};

public:
	MemoryManager();
	~MemoryManager();

	inline static MemoryManager& Instance()
	{
		static MemoryManager instance;
		return instance;
	}

	void*	Allocate(int32 size);
	void	Release(void* ptr);

private:
#ifdef _STOMP
#else
	vector<MemoryPool*> _pools;

	// O(1)을 위한 pool table
	// 1 ~ 4096
	MemoryPool* _pool_table[MAX_ALLOC_SIZE + 1] = {};
#endif
};

template<typename Type, typename ...Args>
Type* xnew(Args&& ...args)
{
	// memory alloc
	Type* malloc_memory = static_cast<Type*>(PoolAllocator::Allocate(sizeof(Type)));	

	// init object with ctor
	new (malloc_memory)Type(std::forward<Args>(args)...);

	return malloc_memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type();	
	PoolAllocator::Release(obj);
}

// 메모리 풀에 shared_ptr 사용
template<typename Type, typename...Args>
shared_ptr<Type> MakeShared(Args&& ...args)
{
	return shared_ptr<Type>{xnew<Type>(forward<Args>(args)...), xdelete<Type>};
}