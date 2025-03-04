#pragma once

class BufferReader
{
public:
	BufferReader();
	BufferReader(BYTE* buffer, uint32 size, uint32 at = 0);
	~BufferReader();

	BYTE* Buffer() const { return _buffer; }
	uint32 Size() const { return _size; }
	uint32 ReadSize() const { return _at; }
	uint32 AvailableSize() const { return _size - _at; }

	template<typename T>
	bool Peek(OUT T* dest) { return Peek(dest, sizeof(T)); }
	bool Peek(OUT void* dest, uint32 len);

	template<typename T>
	bool Read(OUT T* dest) { return Read(dest, sizeof(T)); }
	bool Read(OUT void* dest, uint32 len);

	template<typename T>
	BufferReader& operator>>(OUT T& dest);

private:
	BYTE* _buffer = nullptr;
	uint32 _size = 0;
	uint32 _at = 0;
};

template<typename T>
inline BufferReader& BufferReader::operator>>(T& dest)
{
	dest = *reinterpret_cast<T*>(&_buffer[_at]);
	_at += sizeof(T);
	return *this;
}
