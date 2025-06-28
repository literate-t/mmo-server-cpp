#pragma once

class ChunkPool
{
public:
	ChunkPool(int32 alloc_size);

	void* Pop();
	void Push(void* ptr);

private:
	const int32 _chunk_size;

	vector<void*> _chunks;

	// temp
	int32 _alloc_size;
	int32 _block_count;

	USE_LOCK;
};

int32 CalcChunkSize(int32 alloc_size);
int32 CalcBlockCountOfChunk(int32 alloc_size);
