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
	// ���� ������ �Ҵ翡 �䱸�Ǵ� ũ�Ⱑ size�̰� ���⿡ �޸� ����� ũ�⸦ ���δ�
	const int32 alloc_size = size + sizeof(MemoryHeader);
	// �ִ� ũ�⸦ �ʰ��ϴ� ����� ���ؼ��� Ǯ���� ������� �ʴ´�
	if (MAX_ALLOC_SIZE < alloc_size)
	{
		// new�� ���� ��ü �ʱ�ȭ�� �� �� �Ͼ��
		//memory = new MemoryHeader[alloc_size];
		// Ǯ�� ����� �ƴ϶� ���� 16����Ʈ ������ �� ���൵ ������ �ϰ����� ����
		memory = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(alloc_size, SLIST_ALIGNMENT));
	}
	else
	{
		memory = _pool_table[alloc_size]->Pop();
	}
	// alloc_size�� �޸� ����� ũ�Ⱑ ���ԵǾ� �ִ�
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
		// Ǯ�� �ݳ�
		_pool_table[alloc_size]->Push(memory);
	}
}