#pragma once
#include <stack>


extern thread_local uint32 tls_thread_id;
extern thread_local uint64 tls_end_tick_count;
extern thread_local stack<int32> tls_lock_stack;
extern thread_local SharedSendBufferChunk tls_send_buffer_chunk;
extern thread_local JobQueue* tls_current_job_queue;
extern thread_local class ThreadLocalSlab** tls_slab;