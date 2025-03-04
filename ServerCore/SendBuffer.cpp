#include "pch.h"
#include "SendBuffer.h"

int SendBufferManager::count = 0;

/*----------------
	SendBuffer
----------------*/
SendBuffer::SendBuffer(SharedSendBufferChunk owner_chunk, BYTE* buffer, uint32 alloc_size)
	: _owner_chunk(owner_chunk), _buffer(buffer), _alloc_size(alloc_size)
{
}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::Close(uint32 write_size)
{
	ASSERT_CRASH(write_size <= _alloc_size);
	_write_size = write_size;
	_owner_chunk->Close(_write_size);
}

/*--------------------
	SendBufferChunk
--------------------*/
SendBufferChunk::SendBufferChunk()
{
}

SendBufferChunk::~SendBufferChunk()
{
}

void SendBufferChunk::Reset()
{
	_open = false;
	_used_size = 0;
}

SharedSendBuffer SendBufferChunk::Open(uint32 alloc_size)
{
	ASSERT_CRASH(alloc_size <= CHUNK_SIZE);
	ASSERT_CRASH(IsOpen() == false);

	if (FreeSize() < alloc_size)
		return nullptr;

	SetOpen(true);

	// SendBuffer가 사용할 동안 Chunk가 해제되면 안 되기 때문에 shared_from_this()를 사용	
	return ObjectPool<SendBuffer>::MakeShared(shared_from_this(), Buffer(), alloc_size);
}

void SendBufferChunk::Close(uint32 write_size)
{
	ASSERT_CRASH(true == IsOpen());

	SetOpen(false);
	_used_size += write_size;
}

/*---------------------
	SendBufferManager
---------------------*/
SharedSendBuffer SendBufferManager::Open(uint32 size)
{
	if (nullptr == tls_send_buffer_chunk)
	{
		tls_send_buffer_chunk = Pop();
		tls_send_buffer_chunk->Reset();
		++count;
	}

	ASSERT_CRASH(false == tls_send_buffer_chunk->IsOpen());

	if (tls_send_buffer_chunk->FreeSize() < size)
	{
		tls_send_buffer_chunk = Pop();
		tls_send_buffer_chunk->Reset();
	}

	return tls_send_buffer_chunk->Open(size);
}

SharedSendBufferChunk SendBufferManager::Pop()
{
	{
		WRITE_LOCK;
		if (false == _send_buffer_chunks.empty())
		{
			SharedSendBufferChunk chunk = _send_buffer_chunks.back();
			_send_buffer_chunks.pop_back();
			return chunk;
		}
	}
	return SharedSendBufferChunk(xnew<SendBufferChunk>(), PushGlobal);
}

void SendBufferManager::Push(SharedSendBufferChunk buffer_chunk)
{
	WRITE_LOCK;
	_send_buffer_chunks.push_back(buffer_chunk);
}

void SendBufferManager::PushGlobal(SendBufferChunk* buffer_chunk)
{	
	g_send_buffer_manager->Push(SharedSendBufferChunk(buffer_chunk, PushGlobal));
}
