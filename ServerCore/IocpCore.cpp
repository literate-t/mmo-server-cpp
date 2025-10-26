#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

IocpCore::IocpCore()
{
	_iocp_handle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0);
	ASSERT_CRASH(INVALID_HANDLE_VALUE != _iocp_handle);
}

IocpCore::~IocpCore()
{
	CloseHandle(_iocp_handle);
}

bool IocpCore::Register(SharedIocpObject iocp_object)
{
	// register a socket to iocp without key
	return nullptr != CreateIoCompletionPort(reinterpret_cast<HANDLE>(iocp_object->GetSocket()), _iocp_handle, /*key*/0, 0);
}

bool IocpCore::Dispatch(uint32 timeout_ms)
{
	DWORD number_of_bytes = 0;
	ULONG_PTR key = 0;	
	IocpEvent* iocp_event = nullptr;

	int32 result = GetQueuedCompletionStatus(_iocp_handle, OUT &number_of_bytes, OUT &key, OUT reinterpret_cast<LPOVERLAPPED*>(&iocp_event), timeout_ms);

	if (result)
	{
		SharedIocpObject iocp_object = iocp_event->owner;
		iocp_object->Dispatch(iocp_event, number_of_bytes);
	}
	else
	{
		int32 error = WSAGetLastError();
		switch (error)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			// TODO : logging
			SharedIocpObject iocp_object = iocp_event->owner;
			iocp_object->Dispatch(iocp_event, number_of_bytes);
			break;
		}
	}
	
	return true;
}
