#include "pch.h"

// ------- BaseAllocator ------- //
void* BaseAllocator::Allocate(size_t size)
{
	return malloc(size);
}

void BaseAllocator::Release(void* ptr)
{
	free(ptr);
}

// ------- StompAllocator ------- //
void* StompAllocator::Allocate(size_t size)
{
	const size_t page_count = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	const size_t offset = (page_count * PAGE_SIZE) - size;
	void* base = ::VirtualAlloc(nullptr, page_count * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	return static_cast<void*>(static_cast<int8*>(base) + offset);
}

void StompAllocator::Release(void* ptr)
{
	const int64 current_address = reinterpret_cast<int64>(ptr);
	const int64 base_address = current_address - (current_address % GRANULARITY_SIZE);
	bool result = VirtualFree(reinterpret_cast<void*>(base_address), 0, MEM_RELEASE);	
}

// ------- PoolAllocator ------- //
void* PoolAllocator::Allocate(int32 size)
{
	return PoolManager::Instance().Allocate(size);
}

void PoolAllocator::Release(void* ptr)
{
	return PoolManager::Instance().Release(ptr);
}