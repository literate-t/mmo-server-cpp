#pragma once

enum
{
	SLIST_ALIGNMENT = 16
};

// -------- MemoryHeader -------- //
class alignas(SLIST_ALIGNMENT) MemoryHeader : public SLIST_ENTRY
{
public:
	MemoryHeader() = default;
	MemoryHeader(int32 size) : _alloc_size(size) {}

	static void* Initialize(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size);

		// [MemoryHeader][Data]에서 [Data]의 시작 주소를 반환
		// ++ 연산을 하면 MemoryHeader 크기 만큼 한 단위를 이동
		return reinterpret_cast<void*>(++header);
	}

	static MemoryHeader* Release(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;

		return header;
	}

	int32 _alloc_size;
};

// -------- MemoryPool -------- //
class alignas(SLIST_ALIGNMENT) MemoryPool
{
public:
	MemoryPool(int32 alloc_size);
	~MemoryPool();

	// 메모리 반납
	void			Push(MemoryHeader* header);
	MemoryHeader*	Pop();

private:
	SLIST_HEADER _header;
	int32 _alloc_size = 0;
	atomic<int32> _use_count = 0;
	atomic<int32> _reserve_count = 0;
};
