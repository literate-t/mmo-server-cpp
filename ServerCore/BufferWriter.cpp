#include "pch.h"
#include "BufferWriter.h"

BufferWriter::BufferWriter()
{
}

BufferWriter::BufferWriter(BYTE* buffer, uint32 size, uint32 at)
	:_buffer(buffer), _size(size), _at(at)
{
}

BufferWriter::~BufferWriter()
{
}

bool BufferWriter::Write(void* src, uint32 len)
{
	if (AvailableSize() < len)
		return false;

	memcpy(&_buffer[_at], src, len);
	_at += len;

	return true;
}
