#pragma once
#include <iterator>

const int kFetchCount= 32;
const int kDataCount = 32;
const int kArraySizeCount = 512 / 32;

// ------- CentralPool ------- //
class CentralPool
{
public:
	static void* Allocate(int32 size);
	static void Release(void* ptr);
};

// ------- MemoryStack ------- //ss
/*
* { 32, 48, 64, 80, 96, 112, 128 ..} �� index 0, 1, 2, 3, 4, 5, 6.. 
* �� ������ ���� ������ ũ���� �޸𸮸� ��� �ִ´�
* �޸𸮸� ������ ������ ����ü�� �д�
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
	array<int32, kArraySizeCount> arr_size;
	int32 _size_count;
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