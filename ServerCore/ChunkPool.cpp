#include "pch.h"
#include "ChunkPool.h"

const int32 kBaseChunkSize = 1024 * 16;
const int32 kMallocAlignment = 64;
const int32 kMaxCapacityExpected = 16;

int32 CalcChunkSize(int32 alloc_size)	
{
	int32 g = gcd(alloc_size, kMallocAlignment);
	int32 l = alloc_size / g * kMallocAlignment;

	int32 count = kBaseChunkSize / l;

	return count * l;
}

int32 CalcBlockCountOfChunk(int32 alloc_size)
{
	int32 chunk_size = CalcChunkSize(alloc_size);
	
	return chunk_size / alloc_size;
}

ChunkPool::ChunkPool(int32 alloc_size)	
	:_chunk_size(CalcChunkSize(alloc_size)), _alloc_size(alloc_size), _block_count(CalcBlockCountOfChunk(alloc_size))
{
	_chunks.reserve(kMaxCapacityExpected);	
}

void* ChunkPool::Pop()
{
	WRITE_LOCK;
	if (_chunks.empty())
		// ChunkHeader 크기만큼 더하기
		return _aligned_malloc(_chunk_size + kMallocAlignment, kMallocAlignment);

	void* chunk = _chunks.back();
	_chunks.pop_back();

	return chunk;
}

void ChunkPool::Push(void* ptr)
{
	WRITE_LOCK;
	_chunks.push_back(ptr);
}
