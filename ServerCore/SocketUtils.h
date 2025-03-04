#pragma once
#include "NetAddress.h"

class SocketUtils
{
public:
	static LPFN_CONNECTEX		ConnectEx;
	static LPFN_DISCONNECTEX	DisconnectEx;
	static LPFN_ACCEPTEX		AcceptEx;

	static void Init();
	static void Clear();

	static bool GetWindowExFunction(SOCKET socket, GUID guid, OUT LPVOID* fn);
	static SOCKET CreateAsyncSocket();

	static bool SetLinger(SOCKET socket, uint16 onoff, uint16 linger);
	static bool SetReuseAddress(SOCKET socket, bool flag);
	static bool SetRecvBufferSize(SOCKET socket, int32 size);
	static bool SetSendBufferSize(SOCKET socket, int32 size);
	static bool SetTcpNoDelay(SOCKET socket, bool enable);
	static bool SetUpdatAccpetSocket(SOCKET socket, SOCKET listen_socket);

	static bool Bind(SOCKET socket, NetAddress address);
	static bool BindAnyAddress(SOCKET socket, uint16 port);
	static bool Listen(SOCKET socket, int32 back_log = SOMAXCONN);
	static void Close(SOCKET& socket);
};

template<typename T>
static inline bool SetSockOpt(SOCKET socket, int32 level, int32 optname, T optval)
{
	return SOCKET_ERROR != setsockopt(socket, level, optname, reinterpret_cast<char*>(&optval), sizeof(T));
}