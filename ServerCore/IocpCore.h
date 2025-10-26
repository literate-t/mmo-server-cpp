#pragma once

class IocpKey;
// --------- IocpObject --------- //
class IocpObject : public enable_shared_from_this<IocpObject>
{
public:
	virtual SOCKET GetSocket() abstract;
	virtual void Dispatch(class IocpEvent* iocp_event, int32 number_of_bytes = 0) abstract;
};

// --------- IocpCore --------- //
class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	HANDLE GetHandle() { return _iocp_handle; }

	bool Register(SharedIocpObject iocp_object);
	bool Dispatch(uint32 timeout_ms = INFINITE);

private:
	HANDLE _iocp_handle;
};