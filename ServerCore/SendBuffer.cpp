#include "pch.h"
#include "SendBuffer.h"

int SendBufferManager::count = 0;

// -------- SendBuffer  -------- //
SendBuffer::SendBuffer(SharedSendBufferChunk owner_chunk, BYTE* buffer, uint32 alloc_size)
	: _owner_chunk(owner_chunk), _buffer(buffer), _alloc_size(alloc_size)
{
	OnWrite(_alloc_size);
}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::OnWrite(uint32 write_size)
{
	ASSERT_CRASH(write_size <= _alloc_size);
	_write_size = write_size;
	_owner_chunk->OnWrite(_write_size);
}

// -------- SendBufferChunk  -------- //
SendBufferChunk::SendBufferChunk()
{
}

SendBufferChunk::~SendBufferChunk()
{
}

void SendBufferChunk::Reset()
{
	_used_size = 0;
}

SharedSendBuffer SendBufferChunk::Acquire(uint32 alloc_size)
{
	ASSERT_CRASH(alloc_size <= CHUNK_SIZE);

	if (FreeSize() < alloc_size)
		return nullptr;

	// SendBuffer가 사용할 동안 Chunk가 해제되면 안 되기 때문에 shared_from_this()를 사용	
	return ObjectPool<SendBuffer>::MakeShared(shared_from_this(), Buffer(), alloc_size);
}

void SendBufferChunk::OnWrite(uint32 write_size)
{
	_used_size += write_size;
}

// ----- SendBufferManager ----- //
SharedSendBuffer SendBufferManager::Acquire(uint32 size)
{
	if (nullptr == tls_send_buffer_chunk)
	{
		tls_send_buffer_chunk = Pop();
		tls_send_buffer_chunk->Reset();
		++count;
	}

	if (tls_send_buffer_chunk->FreeSize() < size)
	{
		tls_send_buffer_chunk = Pop();
		tls_send_buffer_chunk->Reset();
	}

	return tls_send_buffer_chunk->Acquire(size);
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
	return SharedSendBufferChunk(xnew<SendBufferChunk>(), [this](SendBufferChunk* buffer_chunk) {this->Push(buffer_chunk);});
}

void SendBufferManager::Push(SendBufferChunk* buffer_chunk)
{
	WRITE_LOCK;
	_send_buffer_chunks.push_back(SharedSendBufferChunk(buffer_chunk, [this](SendBufferChunk* buffer_chunk) {this->Push(buffer_chunk);}));
}
