#pragma once

// ------- ThreadLocalSlabManager ------- //
class ThreadLocalSlabManager
{
public:
	void* allocate(int32 size);
	void deallocate(void* ptr);
};

// ------- ThreadLocalSlab ------- //
class ThreadLocalSlab
{

};