#pragma once

const int32 kMaxChunkInfoCount = 16;
const int32 kChunkStride = 32;
const int32 kMaxBlockSize = 512;
const int32 kThreadSlabArrayCount = kMaxBlockSize / kChunkStride;

// ------- CentralPool ------- //
class CentralPool
{
public:
	static void* Allocate(int32 size);
	static void Release(void* ptr);
};

// ------- MemoryStack ------- //ss
/*
* { 32, 48, 64, 80, 96, 112, 128 ..} → index 0, 1, 2, 3, 4, 5, 6.. 
* 각 사이즈 별로 고정된 크기의 메모리를 들고 있는다
* 메모리를 관리할 별도의 구조체를 둔다
*/
template<int32 N>
struct alignas(64) MemoryStack
{
	inline int32 Top() { return top; }

	void* Pop() { return bufs[--top]; }
	void Push(void* ptr) { bufs[top++] = ptr; }
	bool Empty() { return top == 0; }
	bool Full() { return top == N; }

	void* bufs[N];
	int32 top = 0;
};
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

// ------- ChunkHeader ------- //
class alignas(64) ChunkHeader
{
public:
	ChunkHeader(ChunkInfo* chunk_info)
		: chunk_info(chunk_info) {
	}

	// return: ChunkHeader
	static inline void InitHeader(void* ptr, ChunkInfo* chunk_info)
	{
		ChunkHeader* header = reinterpret_cast<ChunkHeader*>(ptr);
		new(header)ChunkHeader(chunk_info);
	}

	static inline ChunkHeader* DetachHeader(void* ptr)
	{
		ChunkHeader* header = reinterpret_cast<ChunkHeader*>(ptr);
		return --header;
	}

	static inline void* DetachPayload(ChunkHeader* header)
	{
		return ++header;
	}

	static inline void* DetachPayload(ChunkInfo* chunk_info)
	{
		return DetachPayload(reinterpret_cast<ChunkHeader*>(chunk_info->chunk_base));
	}

	ChunkInfo* chunk_info;
};

// ------- BlockHeader ------- //
struct BlockHeader
{
	BlockHeader() = default;
	BlockHeader(ChunkInfo* chunk_info)
		: chunk_info(chunk_info) {
	}

	static inline void* InitHeader(void* ptr, ChunkInfo* chunk_info)
	{
		BlockHeader* header = reinterpret_cast<BlockHeader*>(ptr);
		new(header)BlockHeader(chunk_info);

		return reinterpret_cast<void*>(++header);
	}

	static inline BlockHeader* DetachHeader(void* ptr)
	{
		BlockHeader* header = reinterpret_cast<BlockHeader*>(ptr);
		return --header;
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
	void ReleaseBlock(BlockHeader* header);

private:
	array<class ChunkPool*, kMaxBlockSize + 1> _chunk_pools;
};