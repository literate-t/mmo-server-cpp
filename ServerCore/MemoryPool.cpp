#include "pch.h"
#include "MemoryPool.h"



// -------- MemoryPool -------- //
MemoryPool::MemoryPool(int32 alloc_size): _alloc_size(alloc_size)
{
	::InitializeSListHead(&_header);
}

MemoryPool::~MemoryPool()
{
	while (MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header)))
	{
		::_aligned_free(memory);		
	}
}

void MemoryPool::Push(MemoryHeader* ptr)
{
	ptr->_alloc_size = 0;

	::InterlockedPushEntrySList(&_header, static_cast<PSLIST_ENTRY>(ptr));

	_use_count.fetch_sub(1);
	_reserve_count.fetch_add(1);
}

MemoryHeader* MemoryPool::Pop()
{
	// 원래 데이터로 복구
	MemoryHeader* memory = static_cast<MemoryHeader*>(::InterlockedPopEntrySList(&_header));

	// 메모리가 없다면 새로 만든다
	if (nullptr == memory)
	{
		memory = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(_alloc_size, SLIST_ALIGNMENT));
	}
	else
	{
		ASSERT_CRASH(0 == memory->_alloc_size);
		_reserve_count.fetch_sub(1);
	}

	_use_count.fetch_add(1);

	return memory;
}
