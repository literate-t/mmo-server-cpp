#include "pch.h"

PoolManager::PoolManager()
{
	int32 size = 0;
	int32 size_index = 1;

	for (size = 32; size < 1024; size += 32)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (size_index <= size)
		{
			_pool_table[size_index] = pool;
			++size_index;
		}
	}

	for (; size < 2048; size += 64)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (size_index <= size)
		{
			_pool_table[size_index] = pool;
			++size_index;
		}
	}

	for (; size <= 4096; size += 128)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (size_index <= size)
		{
			_pool_table[size_index] = pool;
			++size_index;
		}
	}
}

PoolManager::~PoolManager()
{
	for (MemoryPool* chunk : _pools)
	{
		delete chunk;
	}

	_pools.clear();
}

void* PoolManager::Allocate(int32 size)
{
	MemoryHeader* memory = nullptr;
	// 실제 데이터 할당에 요구되는 크기가 size이고 여기에 메모리 헤더의 크기를 붙인다
	const int32 alloc_size = size + sizeof(MemoryHeader);
	// 최대 크기를 초과하는 사이즈에 대해서는 풀링을 사용하지 않는다
	if (MAX_ALLOC_SIZE < alloc_size)
	{
		// new를 쓰면 객체 초기화가 두 번 일어난다
		//memory = new MemoryHeader[alloc_size];
		// 풀링 대상이 아니라 굳이 16바이트 정렬을 안 해줘도 되지만 일관성을 위해
		memory = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(alloc_size, SLIST_ALIGNMENT));
	}
	else
	{
		memory = _pool_table[alloc_size]->Pop();
	}
	// alloc_size에 메모리 헤더의 크기가 포함되어 있다
	return MemoryHeader::Initialize(memory, alloc_size);
}

void PoolManager::Release(void* ptr)
{
	MemoryHeader* memory = MemoryHeader::DetachHeader(ptr);
	const int32 alloc_size = memory->_alloc_size;
	ASSERT_CRASH(0 < alloc_size);

	if (MAX_ALLOC_SIZE < alloc_size)
	{
		::_aligned_free(memory);
	}
	else
	{
		// 풀에 반납
		_pool_table[alloc_size]->Push(memory);
	}
}