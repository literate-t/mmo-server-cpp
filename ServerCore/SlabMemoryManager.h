#pragma once

// ------- CentralPool ------- //
class CentralPool
{
public:
	void* allocate(int32 size);
	void deallocate(void* ptr);
};

// ------- ThreadLocalSlab ------- //
class ThreadLocalSlab
{

};