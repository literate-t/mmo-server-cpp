#pragma once

class SlabPool
{
public:
	SlabPool(int32 alloc_size);

	void* Pop();
	void Push(void* ptr);

private:
	const int32 _slab_size;

	vector<void*> _slabs;

	// temp
	int32 _alloc_size;
	int32 _block_count;

	USE_LOCK;
};

int32 CalcSlabkSize(int32 alloc_size);
int32 CalcBlockCountOfSlab(int32 alloc_size);
