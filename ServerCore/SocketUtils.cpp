#include "pch.h"

LPFN_CONNECTEX		SocketUtils::ConnectEx		= nullptr;
LPFN_DISCONNECTEX	SocketUtils::DisconnectEx	= nullptr;
LPFN_ACCEPTEX		SocketUtils::AcceptEx		= nullptr;

void SocketUtils::Init()
{
	WSADATA wsadata;
	ASSERT_CRASH(0 == WSAStartup(MAKEWORD(2, 2), &wsadata));

	// get extended function address
	SOCKET dummy_socket = CreateAsyncSocket();
	ASSERT_CRASH(true == GetWindowExFunction(dummy_socket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
	ASSERT_CRASH(true == GetWindowExFunction(dummy_socket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)));
	ASSERT_CRASH(true == GetWindowExFunction(dummy_socket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));

	Close(dummy_socket);
}

void SocketUtils::Clear()
{
	WSACleanup();
}

bool SocketUtils::GetWindowExFunction(SOCKET socket, GUID guid, OUT LPVOID* fn)
{
	DWORD bytes = 0;
	return SOCKET_ERROR != WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof guid, 
		OUT fn, sizeof *fn, OUT &bytes, nullptr, nullptr);	
}

SOCKET SocketUtils::CreateAsyncSocket()
{
	return WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
}

bool SocketUtils::SetLinger(SOCKET socket, uint16 onoff, uint16 linger)
{
	LINGER optval;
	optval.l_onoff = onoff;
	optval.l_linger = linger;

	return SetSockOpt(socket, SOL_SOCKET, SO_LINGER, optval);
}

bool SocketUtils::SetReuseAddress(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

bool SocketUtils::SetRecvBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
}

bool SocketUtils::SetSendBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
}

bool SocketUtils::SetTcpNoDelay(SOCKET socket, bool enable)
{
	return SetSockOpt(socket, IPPROTO_TCP, TCP_NODELAY, enable);
}

bool SocketUtils::SetUpdatAccpetSocket(SOCKET socket, SOCKET listen_socket)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listen_socket);
}

bool SocketUtils::Bind(SOCKET socket, NetAddress address)
{
	return SOCKET_ERROR != ::bind(socket, reinterpret_cast<sockaddr*>(&address.GetSockAddrIn()), sizeof sockaddr);
}

bool SocketUtils::BindAnyAddress(SOCKET socket, uint16 port)
{
	SOCKADDR_IN address;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(INADDR_ANY);

	return SOCKET_ERROR != ::bind(socket, (sockaddr*)&address, sizeof address);
}

bool SocketUtils::Listen(SOCKET socket, int32 back_log)
{
	return SOCKET_ERROR != listen(socket, back_log);
}

void SocketUtils::Close(SOCKET& socket)
{
	if (INVALID_SOCKET != socket)
		closesocket(socket);
	socket = INVALID_SOCKET;
}
