#include "pch.h"
#include "Session.h"
#include "Service.h"

// -------- Session -------- //
Session::Session() : _recv_buffer{ BUFFER_SIZE }
{
	_socket = SocketUtils::CreateAsyncSocket();
	ASSERT_CRASH(INVALID_SOCKET != _socket);
}

Session::~Session()
{
	printf("Session close: %llu\n", _socket);
	SocketUtils::Close(_socket);	
}

void Session::Send(SharedSendBuffer send_buffer)
{
	if (false == IsConnected())
		return;

	// ���� RegisterSend�� �ɸ��� ���� ������ �� �ɾ��ش�
	// ���� ���ǿ� ���� ���� ȣ��
	{
		WRITE_LOCK;
		_send_queue.push(send_buffer);
	}

	if (false == _send_registered.exchange(true))
		RegisterSend();
}

void Session::Send(xqueue<SharedSendBuffer>& send_buffers)
{
	if (false == IsConnected())
		return;

	{
		WRITE_LOCK;
		while (!send_buffers.empty())
		{
			_send_queue.push(move(send_buffers.front()));
			send_buffers.pop();
		}
	}

	if (false == _send_registered.exchange(true))
		RegisterSend();
}

bool Session::Connect()
{
	return RegisterConnect();
}

void Session::Disconnect(const WCHAR* cause)
{
	if (_connected.exchange(false) == false)
		return;

	printf("Disconnect: %ls\n", cause);
	RegisterDisconnect();
}

SOCKET Session::GetSocket()
{
	return _socket;
}

void Session::Dispatch(IocpEvent* iocp_event, int32 number_of_bytes)
{

	switch (iocp_event->GetType())
	{
	case EventType::Connect:
		EstablishConnect(iocp_event);
		break;
	case EventType::Disconnect:
		ProcessDisconnect(iocp_event);
		break;
	case EventType::Recv:
		ProcessRecv(number_of_bytes);
		break;
	case EventType::Send:
		ProcessSend(number_of_bytes);
		break;
	default:
		break;
	}
}

bool Session::RegisterDisconnect()
{
	DisconnectEvent* disconnect_event = xnew<DisconnectEvent>();
	disconnect_event->Init();
	disconnect_event->owner = shared_from_this();
	
	bool result = SocketUtils::DisconnectEx(_socket, disconnect_event, 0, 0);
	const int32 error = WSAGetLastError();
	if (false == result && error != WSA_IO_PENDING)
	{
		disconnect_event->owner = nullptr;
		xdelete(disconnect_event);
		return false;
	}

	return true;
}

void Session::ProcessDisconnect(IocpEvent* disconnect_event)
{
	disconnect_event->owner = nullptr;
	_connected.store(false);
	OnDisconnectCompleted();
	GetService()->UnregisterSession(GetSharedSession());
	GetService()->RemoveFromSessionManager(GetSharedSession());
	xdelete(disconnect_event);
}

bool Session::RegisterConnect()
{
	if (true == IsConnected())
		return false;

	if (ServiceType::Client != GetService()->GetServiceType())
		return false;

	if (false == SocketUtils::SetReuseAddress(_socket, true))
		return false;

	// ConnectEx �Լ��� ����� �� ���ε��� �ʼ�
	if (false == SocketUtils::BindAnyAddress(_socket, 0))
		return false;

	ConnectEvent* connect_event = xnew<ConnectEvent>();
	connect_event->Init();
	connect_event->owner = shared_from_this();

	DWORD bytes = 0;
	SOCKADDR_IN sock_addr = GetService()->GetNetAddress().GetSockAddrIn();
	bool result = SocketUtils::ConnectEx(_socket, reinterpret_cast<sockaddr*>(&sock_addr), sizeof sock_addr, nullptr, 0, &bytes, connect_event);
	if (false == result && WSAGetLastError() != WSA_IO_PENDING)
	{
		connect_event->owner = nullptr;
		xdelete(connect_event);
		return false;
	}

	return true;
}

// ������ ������ ������ Ŭ���̾�Ʈ�� ���ؼ� ���� �ڵ�������
// ������ ���� �ٸ� ������ ������ ���� �������� ����ϵ��� ����
void Session::EstablishConnect(IocpEvent* connect_event)
{
	_connected.store(true);
	// ���� Ŭ�� ������ �� �ʿ��� �ڵ�
	if (nullptr != connect_event)
	{
		connect_event->owner = nullptr;
		xdelete(connect_event);
	}

	GetService()->RegisterSession(GetSharedSession());
	GetService()->AddToSessionManager(GetSharedSession());

	// ������ �ܿ��� ����
	OnConnectCompleted();

	// ���� ����
	RegisterRecv();
}

void Session::RegisterRecv()
{
	if (false == IsConnected())
		return;

	WSABUF wsabuf;
	wsabuf.buf = reinterpret_cast<char*>(_recv_buffer.WriteAt());
	wsabuf.len = _recv_buffer.FreeSize();

	DWORD recv_bytes = 0;
	DWORD flags = 0;

	// add_ref
	_recv_event.Init();
	_recv_event.owner = shared_from_this();

	int32 result = WSARecv(_socket, &wsabuf, 1, OUT &recv_bytes, OUT &flags, static_cast<LPWSAOVERLAPPED>(&_recv_event), nullptr);
	int32 error = WSAGetLastError();
	if (SOCKET_ERROR == result && WSA_IO_PENDING != WSAGetLastError())
	{
		HandleError(error);
		// release_ref
		_recv_event.owner = nullptr;
		return;
	}
}

void Session::ProcessRecv(int32 recv_bytes)
{
	_recv_event.owner = nullptr;
	if (0 == recv_bytes)
	{
		Disconnect(L"Recv zero");
		return;
	}

	if (false == _recv_buffer.OnWrite(recv_bytes))
	{
		Disconnect(L"OnWrite overflow");
		return;
	}
	
	int32 process_size = OnRecvCompleted(_recv_buffer.ReadAt(), recv_bytes);
																	// process_size��ŭ�� Ŀ�� �̵�
	if (process_size < 0 || recv_bytes < process_size || false == _recv_buffer.OnRead(process_size))
	{
		Disconnect(L"OnRecv error");
		return;
	}
	
	// rw offset ����
	_recv_buffer.Clear();

	RegisterRecv();
}

void Session::RegisterSend()
{
	if (IsConnected() == false)
		return;

	_send_event.Init();
	_send_event.owner = shared_from_this();

	// ���� �����͸� send_event�� ���
	// ���۷��� ī��Ʈ ������ ���� SendEvent�� ��������� �̿��Ѵ�
	{
		WRITE_LOCK;
		int32 write_size = 0;
		if (_send_queue.empty())
		{
			_send_registered.store(false);
			return;
		}

		while (false == _send_queue.empty())
		{
			SharedSendBuffer send_buffer = _send_queue.front();
			_send_queue.pop();
			write_size += send_buffer->WriteSize();

			_send_event.send_buffers.push_back(send_buffer);
		}
	}

	xvector<WSABUF> wsabufs;
	wsabufs.reserve(_send_event.send_buffers.size());
	for (SharedSendBuffer& send_buffer : _send_event.send_buffers)
	{
		WSABUF wsabuf;
		wsabuf.buf = reinterpret_cast<char*>(send_buffer->Buffer());
		wsabuf.len = static_cast<LONG>(send_buffer->WriteSize());	

		wsabufs.push_back(wsabuf);
	}

	DWORD send_bytes = 0;

	int32 result = WSASend(_socket, wsabufs.data(), static_cast<DWORD>(wsabufs.size()), &send_bytes, 0, &_send_event, nullptr);
	int32 error = WSAGetLastError();
	if (SOCKET_ERROR == result && error != WSA_IO_PENDING)
	{
		HandleError(error);
		// release ref
		_send_event.owner = nullptr;
		_send_event.send_buffers.clear();
		_send_registered.store(false);
		return;
	}
}

void Session::ProcessSend(int32 send_bytes)
{
	// release ref
	_send_event.owner = nullptr;
	_send_event.send_buffers.clear();

	if (0 == send_bytes)
	{
		Disconnect(L"Send zero");
		return;
	}

	// ������ �ܿ��� �������̵�
	OnSendCompleted(send_bytes);

	{
		WRITE_LOCK;
		// ���� ����
		// ���� ���ǿ� ���� ���� ȣ��
		if (_send_queue.empty())
		{
			_send_registered.store(false);
			return;
		}
	}

	RegisterSend();
}

void Session::HandleError(int32 error)
{
	switch(error)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"Error occurs");
		break;
	default:
		// TODO : logging
		// �α��� ����ϴ� ���� �����忡 �ѱ�� ����� ����غ� ��
		puts("error!");
	}
}

// -------- PacketSession -------- //
PacketSession::PacketSession()
{
}

PacketSession::~PacketSession()
{
}

int32 PacketSession::OnRecvCompleted(BYTE* buffer, int32 length)
{
	int32 process_length = 0;
	while (true)
	{
		int32 data_size = length - process_length;

		// ����� �Ľ��� �� �ִ°�
		if (data_size < sizeof PacketHeader)
			break;

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[process_length]));
		if (data_size < header.size)
			break;

		// contents(GameSession)
		OnRecvPacketProcessed(&buffer[process_length], header.size);

		process_length += header.size;
	}

	return process_length;
}
