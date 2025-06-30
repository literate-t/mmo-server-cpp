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

// ------- ThreadLocalSlab ------- //

class alignas(64) ThreadLocalSlab
{
public:
	void* Allocate(int32 size);
	void Release(void* ptr);

private:
	void Fetch(int32 index);
	void Flush(int32 index);

private:
	array<MemoryStack<kDataCount>, kArraySizeCount> _memory_stacks;
};