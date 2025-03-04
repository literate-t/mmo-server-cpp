#pragma once

class BufferWriter
{
public:
	BufferWriter();
	BufferWriter(BYTE* buffer, uint32 size, uint32 at = 0);
	~BufferWriter();

	BYTE* Buffer() const { return _buffer; }
	uint32 Size() const { return _size; }
	uint32 WriteSize() const { return _at; }
	uint32 AvailableSize() const { return _size - _at; }

	template<typename T>
	bool Write(T* src) { return Write(src, sizeof(T)); }
	bool Write(void* src, uint32 len);

	template<typename T>
	T* Reserve(uint16 count = 1);

	//template<typename T>
	//BufferWriter& operator<<(const T& src);

	template<typename T>
	BufferWriter& operator<<(T&& src);

private:
	BYTE* _buffer = nullptr;
	uint32 _size = 0;
	uint32 _at = 0;
};

template<typename T>
inline T* BufferWriter::Reserve(uint16 count)
{
	if (AvailableSize() < sizeof(T) * count)
		return nullptr;

	T* buffer = reinterpret_cast<T*>(&_buffer[_at]);
	_at += (sizeof(T) * count);

	return buffer;
}

//template<typename T>
//inline BufferWriter& BufferWriter::operator<<(const T& src)
//{
//	*reinterpret_cast<T*>(&_buffer[_at]) = src;
//	_at += sizeof(T);
//
//	return *this;
//}

template<typename T>
inline BufferWriter& BufferWriter::operator<<(T&& src)
{
	// 아무 작업을 하지 않으면 T& 일 때 포인터로 변환이 안 된다
	using Type = remove_reference_t<T>;
	*reinterpret_cast<Type*>(&_buffer[_at]) = std::forward<T>(src);
	_at += sizeof(T);

	return *this;
}

