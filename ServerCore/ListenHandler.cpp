#include "pch.h"
#include "ListenHandler.h"
#include "NetAddress.h"
#include "SocketUtils.h"
#include "IocpEvent.h"
#include "Session.h"
#include "Service.h"

ListenHandler::~ListenHandler()
{
	SocketUtils::Close(_socket);

	for (AcceptEvent* event : _accept_events)
	{
		// TODO
		xdelete(event);
	}	
}

bool ListenHandler::StartAccept(SharedService server_service)
{
	_service = server_service;
	if (nullptr == _service)
		return false;

	_socket = SocketUtils::CreateAsyncSocket();
	if (INVALID_SOCKET == _socket)
		return false;

	// register a listen socket to iocp
	if (false == _service->GetIocpCore()->Register(shared_from_this()))
		return false;

	if (false == SocketUtils::SetReuseAddress(_socket, true))
		return false;

	if (false == SocketUtils::SetLinger(_socket, 0, 0))
		return false;

	if (false == SocketUtils::Bind(_socket, _service->GetNetAddress()))
		return false;

	if (false == SocketUtils::Listen(_socket))
		return false;

	const int32 accept_count = _service->GetMaxSessionCount();
	for (int32 i = 0; i < accept_count; ++i)
	{
		AcceptEvent* accept_event = xnew<AcceptEvent>();
		// shared_ptr<ListenHandler>가 owner에 의해 생명이 보장된다
		accept_event->owner = shared_from_this();
		_accept_events.push_back(accept_event);
		RegisterAccept(accept_event);
	}

	return true;
}

void ListenHandler::CloseSocket()
{
	SocketUtils::Close(_socket);
}

SOCKET ListenHandler::GetSocket()
{
	return _socket;
}

void ListenHandler::Dispatch(IocpEvent* iocp_event, int32 number_of_bytes)
{
	ASSERT_CRASH(iocp_event->GetType() == EventType::Accept);
	AcceptEvent* accept_event = static_cast<AcceptEvent*>(iocp_event);
	SetupSession(accept_event);
}

void ListenHandler::RegisterAccept(AcceptEvent* accept_event)
{
	SharedSession session = _service->CreateSessionToIocp();
	accept_event->Init();
	accept_event->session = session;
	DWORD bytes_recved = 0;

	if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recv_buffer.WriteAt(), 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
		OUT &bytes_recved, static_cast<LPOVERLAPPED>(accept_event)))
	{
		const int32 error = WSAGetLastError();
		if (WSA_IO_PENDING != error)
		{			
			RegisterAccept(accept_event);
		}
	}
}

void ListenHandler::SetupSession(AcceptEvent* accept_event)
{	
	SharedSession session = accept_event->session;

	if (nullptr == session || false == SocketUtils::SetUpdatAccpetSocket(session->GetSocket(), _socket))
	{
		RegisterAccept(accept_event);
		return;
	}	

	SOCKADDR_IN sock_address;
	int32 sock_address_size = sizeof sock_address;
	if (SOCKET_ERROR == getpeername(session->GetSocket(), OUT reinterpret_cast<sockaddr*>(&sock_address), &sock_address_size))
	{
		RegisterAccept(accept_event);
		return;
	}

	session->SetAddress(NetAddress(sock_address));
	session->EstablishConnect(nullptr);

	RegisterAccept(accept_event);	
}
