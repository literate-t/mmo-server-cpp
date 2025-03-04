#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(int32 buffer_size) : _buffer_size(buffer_size)
{
	_capacity = _buffer_size * BUFFER_COUNT;
	_buffer.resize(_capacity);
}

RecvBuffer::~RecvBuffer()
{
}

void RecvBuffer::Clear()
{
	int32 data_size = DataSizeToRead();
	if (0 == data_size)
	{
		_read_at = _write_at = 0;
		return;
	}
	
	if (FreeSize() < _buffer_size)
	{
		memcpy(&_buffer[0], &_buffer[_read_at], data_size);
		_read_at = 0;
		_write_at = data_size;
	}
}

bool RecvBuffer::OnRead(int32 read_bytes)
{
	if (DataSizeToRead() < read_bytes)
		return false;

	_read_at += read_bytes;

	return true;
}

bool RecvBuffer::OnWrite(int32 write_bytes)
{
	if (FreeSize() < write_bytes)
		return false;

	_write_at += write_bytes;

	return true;
}
