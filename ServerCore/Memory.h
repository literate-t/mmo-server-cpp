#pragma once

class MemoryPool;

/*-------------------
	MemoryManager
-------------------*/
class MemoryManager
{
	enum
	{
		// ~1024������ 32����, ~2048������ 128����, ~4096���� 256����
		// ������ �������� �����ϰ�
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		MAX_ALLOC_SIZE = 4096
	};

public:
	MemoryManager();
	~MemoryManager();

	void*	Allocate(int32 size);
	void	Release(void* ptr);

private:
#ifdef _STOMP
#else
	vector<MemoryPool*> _pools;

	// O(1)�� ���� pool table
	// 0 ~ 4096 ������ ����
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

// �޸� Ǯ�� shared_ptr ���
template<typename Type, typename...Args>
shared_ptr<Type> MakeShared(Args&& ...args)
{
	return shared_ptr<Type>{xnew<Type>(forward<Args>(args)...), xdelete<Type>};
}