#include "pch.h"
#include "BufferReader.h"

BufferReader::BufferReader()
{
}

BufferReader::BufferReader(BYTE* buffer, uint32 size, uint32 at)
	:_buffer(buffer), _size(size), _at(at)
{

}

BufferReader::~BufferReader()
{
}

bool BufferReader::Peek(void* dest, uint32 len)
{
	if (AvailableSize() < len)
		return false;

	memcpy(dest, &_buffer[_at], len);

	return true;
}

bool BufferReader::Read(void* dest, uint32 len)
{
	if (false == Peek(dest, len))
		return false;

	_at += len;
	return true;
}
