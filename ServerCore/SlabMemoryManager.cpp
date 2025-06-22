#include "pch.h"
#include "SlabMemoryManager.h"

// ------- CentralPool ------- //
void* CentralPool::Allocate(int32 size)
{
    return PoolManager::Instance().Allocate(size);
}

void CentralPool::Release(void* ptr)
{
    PoolManager::Instance().Release(ptr);
}

// ------- ThreadLocalSlab ------- //
void* ThreadLocalSlab::Allocate(int32 size)
{
    int32 index = SizeConverter::Instance().ToIndex(size);
    auto& stack = _memory_stacks[index];

    // 스택이 비어 있으면 중앙 풀에서 새로 받아온다
    if (stack.Empty())
        Fetch(index);

    return stack.Pop();
}

void ThreadLocalSlab::Release(void* ptr)
{
    MemoryHeader* header = MemoryHeader::DetachHeader(ptr);
    int32 index = SizeConverter::Instance().ToIndex(header->_alloc_size);
    auto& stack = _memory_stacks[index];

    if (stack.Full())
        Flush(index);

    stack.Push(ptr);
}

void ThreadLocalSlab::Fetch(int32 index)
{
    auto& stack = _memory_stacks[index];
    int32 size = SizeConverter::Instance().GetArray()[index];

    for (int i = 0; i < kFetchCount - stack.top; ++i)
        stack.Push(CentralPool::Allocate(size));
}

void ThreadLocalSlab::Flush(int32 index)
{
    int32 size = SizeConverter::Instance().GetArray()[index];
    auto& stack = _memory_stacks[index];

    int32 to = stack.top / 2;
    for (int32 i = 0; i < to; ++i)
        CentralPool::Release(stack.Pop());
}

// ------- SizeConverter ------- //
SizeConverter::SizeConverter()    
{
    generate(arr_size.begin(), arr_size.end(), []() { static int32 seed; return seed += 32; });
    _size_count = static_cast<int32>(arr_size.size());
}

int32 SizeConverter::ToIndex(int32 size)
{
    auto find_iter = lower_bound(arr_size.begin(), arr_size.end(), size);
    return static_cast<int32>(distance(arr_size.begin(), find_iter));
}
