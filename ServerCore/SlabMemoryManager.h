#pragma once

const int32 kMaxChunkInfoCount = 16;
const int32 kChunkStride = 32;
const int32 kMaxBlockSize = 512;
const int32 kThreadSlabArrayCount = kMaxBlockSize / kChunkStride;

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

struct ChunkInfo
{
	ChunkInfo(void* chunk_base, int32 alloc_size, int32 block_count)
		: chunk_base(chunk_base), alloc_size(alloc_size), block_count(block_count)
	{
		freed_count.store(0, memory_order_release);
	}

	void* chunk_base;
	int32 alloc_size;
	int32 block_count;
	atomic<int32> freed_count;
};

// ------- ChunkMetadata ------- //
class alignas(64) ChunkMetadata
{
public:
	ChunkMetadata(ChunkInfo* chunk_info)
		: chunk_info(chunk_info) {
	}

	// return: Payload
	static inline void* InitHeader(void* ptr, ChunkInfo* chunk_info)
	{
		ChunkMetadata* header = reinterpret_cast<ChunkMetadata*>(ptr);
		new(header)ChunkMetadata(chunk_info);

		return ++header;
	}

	static inline ChunkMetadata* DetachHeader(void* ptr)
	{
		ChunkMetadata* header = reinterpret_cast<ChunkMetadata*>(ptr);
		return --header;
	}

	static inline void* DetachPayload(ChunkMetadata* header)
	{
		return ++header;
	}

	static inline void* DetachPayload(ChunkInfo* chunk_info)
	{
		return DetachPayload(reinterpret_cast<ChunkMetadata*>(chunk_info->chunk_base));
	}

	ChunkInfo* chunk_info;
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
	void Flush(ChunkInfo* chunk_info);

private:
	const int32 _block_count;

	xvector<ChunkInfo*> _chunk_infos;
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

	ChunkInfo* RefillChunk(int32 block_size);
	void DrainChunk(void* chunk_base);

private:
	void* AcquireBlock(int32 size);
	void ReleaseBlock(ChunkMetadata* header);

private:
	array<class ChunkPool*, kMaxBlockSize + 1> _chunk_pools;
};