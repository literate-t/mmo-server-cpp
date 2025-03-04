#pragma once
class RecvBuffer
{
	enum { BUFFER_COUNT = 10};
public:
	RecvBuffer(int32 buffer_size);
	~RecvBuffer();

	void Clear();
	bool OnRead(int32 read_bytes);
	bool OnWrite(int32 write_bytes);

	BYTE* ReadAt() { return &_buffer[_read_at]; }
	BYTE* WriteAt() { return &_buffer[_write_at]; }
	int32 DataSizeToRead() const { return _write_at - _read_at; }
	int32 FreeSize() const { return _capacity - _write_at; }

private:
	// 한 번에 쓸 수 있는 최대 사이즈
	int32 _buffer_size = 0;
	int32 _capacity = 0;
	int32 _read_at = 0;
	int32 _write_at = 0;
	xvector<BYTE> _buffer{};
};

