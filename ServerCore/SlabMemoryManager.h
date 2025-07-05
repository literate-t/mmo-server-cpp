#pragma once

const int32 kMaxSlabInfoCount = 16;
const int32 kBlockStride = 32;
const int32 kMaxBlockSize = 512;
const int32 kThreadSlabArrayCount = kMaxBlockSize / kBlockStride;

// ------- SizeConverter ------- //
class SizeConverter
{
public:
	SizeConverter();

	inline static SizeConverter& Instance()
	{
		static SizeConverter instance;
		return instance;
	}

	inline int32 ToIndex(int32 size)
	{
		ASSERT_CRASH(size > 0 && size <= kMaxBlockSize);
		return _size_converter[size];
	}

	constexpr auto& GetArray() const { return arr_size; }

private:
	array<int32, kThreadSlabArrayCount> arr_size;
	array<int32, kMaxBlockSize + 1> _size_converter;
};

struct SlabInfo
{
	SlabInfo(void* slab_base, int32 alloc_size, int32 block_count)
		: slab_base(slab_base), alloc_size(alloc_size), block_count(block_count)
	{
		freed_count.store(0, memory_order_release);
	}

	void* slab_base;
	int32 alloc_size;
	int32 block_count;
	atomic<int32> freed_count;
};

// ------- SlabMetadata ------- //
class alignas(64) SlabMetadata
{
public:
	SlabMetadata(SlabInfo* slab_info)
		: slab_info(slab_info) {
	}

	// return: Payload
	static inline void* InitHeader(void* ptr, SlabInfo* slab_info)
	{
		SlabMetadata* header = reinterpret_cast<SlabMetadata*>(ptr);
		new(header)SlabMetadata(slab_info);

		return ++header;
	}

	static inline SlabMetadata* DetachHeader(void* ptr)
	{
		SlabMetadata* header = reinterpret_cast<SlabMetadata*>(ptr);
		return --header;
	}

	static inline void* DetachPayload(SlabMetadata* header)
	{
		return ++header;
	}

	static inline void* DetachPayload(SlabInfo* slab_info)
	{
		return DetachPayload(reinterpret_cast<SlabMetadata*>(slab_info->slab_base));
	}

	SlabInfo* slab_info;
};

// ------- ThreadLocalSlab ------- //
class ThreadLocalSlab
{
public:
	ThreadLocalSlab() = default;
	ThreadLocalSlab(int32 block_size);
	~ThreadLocalSlab();

	void* Acquire();
	void Release();

	const int32 _block_size;
private:
	void Fetch();
	void Flush(SlabInfo* slab_info);

private:
	const int32 _block_count;

	xvector<SlabInfo*> _slab_infos;
	xvector<void*> _free_block_payloads;
};

// ------- SlabMemoryManager ------- //
class SlabMemoryManager
{
public:
	SlabMemoryManager();
	~SlabMemoryManager();

	inline static SlabMemoryManager& Instance()
	{
		static SlabMemoryManager instance;
		return instance;
	}

	void* Acquire(int32 size);
	void Release(void* block_payload);

	SlabInfo* RefillSlab(int32 block_size);
	void DrainSlab(void* slab_base);

private:
	void* AcquireBlock(int32 size);
	void ReleaseBlock(SlabMetadata* header);

private:
	array<class SlabPool*, kMaxBlockSize + 1> _slab_pools;
};