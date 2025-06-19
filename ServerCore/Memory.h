#pragma once

class MemoryPool;
class PoolManager
{
	enum
	{
		// ~1024������ 32����, ~2048������ 128����, ~4096���� 256������ ����
		// ������ �������� �����ϰ�
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		MAX_ALLOC_SIZE = 4096
	};

public:
	PoolManager();
	~PoolManager();

	inline static PoolManager& Instance()
	{
		static PoolManager instance;
		return instance;
	}

	void*	Allocate(int32 size);
	void	Release(void* ptr);

private:
	vector<MemoryPool*> _pools;

	// O(1)�� ���� pool table
	// 1 ~ 4096
	MemoryPool* _pool_table[MAX_ALLOC_SIZE + 1] = {};
};

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

// �޸� Ǯ�� shared_ptr ���
template<typename Type, typename...Args>
shared_ptr<Type> MakeShared(Args&& ...args)
{
	return shared_ptr<Type>{xnew<Type>(forward<Args>(args)...), xdelete<Type>};
}