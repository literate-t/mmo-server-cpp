#pragma once
class NetAddress
{
public:
	NetAddress() = default;
	NetAddress(SOCKADDR_IN sock_addr);
	NetAddress(wstring ip, uint16 port);

	SOCKADDR_IN&	GetSockAddrIn() { return _sock_addr; }
	wstring			GetIp();
	uint16			GetPort() { return ntohs(_sock_addr.sin_port); }

	static IN_ADDR IpToAddress(const WCHAR* ip);

private:
	SOCKADDR_IN _sock_addr {};
};