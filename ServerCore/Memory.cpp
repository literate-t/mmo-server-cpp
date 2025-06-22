#include "pch.h"
#include "SlabMemoryManager.h"

void* tls_pool_alloc(int32 size)
{
	return tls_slab.Allocate(size);
}

void tls_pool_free(void* ptr)
{
	tls_slab.Release(ptr);
}