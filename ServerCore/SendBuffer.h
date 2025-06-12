#pragma once

/*
* 스레드마다 Chunk가 존재하고 각 스레드에서 필요한 만큼
* SendBuffer를 할당받아 사용한다
*/

// -------- SendBuffer  -------- //
class SendBuffer : public enable_shared_from_this<SendBuffer>
{
public:
	SendBuffer(SharedSendBufferChunk owner_chunk, BYTE* buffer, uint32 alloc_size);
	~SendBuffer();

	BYTE* Buffer() { return _buffer; }
	uint32 WriteSize() const { return _write_size; }
	uint32 AllocSize() const { return _alloc_size; }

private:
	void OnWrite(uint32 write_size);	

private:
	BYTE* _buffer;	
	uint32 _alloc_size = 0;
	uint32 _write_size = 0;
	SharedSendBufferChunk _owner_chunk;
};


// -------- SendBufferChunk  -------- //
class SendBufferChunk : public enable_shared_from_this<SendBufferChunk>
{
	friend class SendBufferManager;
	friend SendBuffer;

	enum
	{
		CHUNK_SIZE = 0x2000
	};

public:
	SendBufferChunk();
	~SendBufferChunk();

private:
	void Reset();
	SharedSendBuffer Acquire(uint32 alloc_size);
	void OnWrite(uint32 write_size);

	inline uint32 UsedSize() const { return _used_size; }
	inline BYTE* Buffer() { return &_buffer[_used_size]; }
	inline uint32 FreeSize() { return static_cast<uint32>(_buffer.size() - _used_size); }
	
private:
	Array<BYTE, CHUNK_SIZE> _buffer{};
	uint32 _used_size = 0;
};

// pool 역할
// ----- SendBufferManager ----- //
class SendBufferManager
{
public:
	SharedSendBuffer Acquire(uint32 size);	

private:
	SharedSendBufferChunk Pop();
	void Push(SendBufferChunk* buffer_chunk);	

private:
	USE_LOCK;
	xvector<SharedSendBufferChunk> _send_buffer_chunks;
	static int count;
};