#include "pch.h"
#include "SlabMemoryManager.h"

thread_local uint32 tls_thread_id = 0;
thread_local uint64 tls_end_tick_count = 0;
thread_local stack<int32> tls_lock_stack;
thread_local SharedSendBufferChunk tls_send_buffer_chunk;
thread_local JobQueue* tls_current_job_queue = nullptr;
thread_local ThreadLocalSlab** tls_slab = []() {
	ThreadLocalSlab** arr = new ThreadLocalSlab*[kThreadSlabArrayCount];

	for (int i = 0; i < kThreadSlabArrayCount; ++i)
		arr[i] = new ThreadLocalSlab(SizeConverter::Instance().GetArray()[i]);

	return arr;
	}();