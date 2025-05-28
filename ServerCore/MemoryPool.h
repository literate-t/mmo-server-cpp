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

		// [MemoryHeader][Data]���� [Data]�� ���� �ּҸ� ��ȯ
		// ++ ������ �ϸ� MemoryHeader ũ�� ��ŭ �� ������ �̵�
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

	// �޸� �ݳ�
	void			Push(MemoryHeader* header);
	MemoryHeader*	Pop();

private:
	SLIST_HEADER _header;
	int32 _alloc_size = 0;
	atomic<int32> _use_count = 0;
	atomic<int32> _reserve_count = 0;
};
