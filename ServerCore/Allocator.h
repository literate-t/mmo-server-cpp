#pragma once

/*-------------------
	BaseAllocator
-------------------*/
class BaseAllocator
{
public:
	static void* Allocate(size_t size);
	static void Release(void* ptr);
};

/*-------------------
	StompAllocator
-------------------*/
class StompAllocator
{
	enum { 
		PAGE_SIZE = 0x1000,
		GRANULARITY_SIZE = 0x10000
	};
public:
	static void* Allocate(size_t size);
	static void Release(void* ptr);
};

/*-------------------
	PoolAllocator
-------------------*/
class PoolAllocator
{
	enum { PAGE_SIZE = 0x1000 };
public:
	static void* Allocate(int32 size);
	static void Release(void* ptr);
};

/*-------------------
	 StlAllocator
-------------------*/
template<typename T>
class StlAllocator
{
public:
	using value_type = T;

	StlAllocator() {}

	template<typename U>
	StlAllocator(const StlAllocator<U>&) {}

	T* allocate(size_t count)
	{
		const int32 size = static_cast<int32>(count * sizeof(value_type));
		// count가 1로 들어오는 이유는 내부적으로 메모리 할당을 위한 과정
		// value_type에는 struct std::_Container_proxy 타입이 들어온다
		return static_cast<T*>(PoolAllocator::Allocate(size));
	}

	void deallocate(T* ptr, size_t count)
	{
		PoolAllocator::Release(ptr);
	}

	// swap 연산을 위해 추가
	template<typename U>
	bool operator==(const StlAllocator<U>&) { return true; }
};