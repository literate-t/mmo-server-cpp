#include "pch.h"
#include "SlabPool.h"

const int32 kBaseSlabSize = 1024 * 16;
const int32 kMallocAlignment = 64;
const int32 kMaxCapacityExpected = 16;

int32 CalcSlabkSize(int32 alloc_size)	
{
	int32 g = gcd(alloc_size, kMallocAlignment);
	int32 l = alloc_size / g * kMallocAlignment;

	int32 count = kBaseSlabSize / l;

	return count * l;
}

int32 CalcBlockCountOfSlab(int32 alloc_size)
{
	int32 slab_size = CalcSlabkSize(alloc_size);
	
	return slab_size / alloc_size;
}

SlabPool::SlabPool(int32 alloc_size)	
	:_slab_size(CalcSlabkSize(alloc_size)), _alloc_size(alloc_size), _block_count(CalcBlockCountOfSlab(alloc_size))
{
	_slabs.reserve(kMaxCapacityExpected);	
}

void* SlabPool::Pop()
{
	WRITE_LOCK;
	if (_slabs.empty())
		// ChunkMetadata 크기만큼 더하기
		return _aligned_malloc(_slab_size + kMallocAlignment, kMallocAlignment);

	void* chunk = _slabs.back();
	_slabs.pop_back();

	return chunk;
}

void SlabPool::Push(void* ptr)
{
	WRITE_LOCK;
	_slabs.push_back(ptr);
}
