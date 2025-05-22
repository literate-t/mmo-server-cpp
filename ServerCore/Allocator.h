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
		// count�� 1�� ������ ������ ���������� �޸� �Ҵ��� ���� ����
		// value_type���� struct std::_Container_proxy Ÿ���� ���´�
		return static_cast<T*>(PoolAllocator::Allocate(size));
	}

	void deallocate(T* ptr, size_t count)
	{
		PoolAllocator::Release(ptr);
	}

	// swap ������ ���� �߰�
	template<typename U>
	bool operator==(const StlAllocator<U>&) { return true; }
};