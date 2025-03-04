#include "pch.h"
#include "NetAddress.h"

NetAddress::NetAddress(SOCKADDR_IN sock_addr) : _sock_addr(sock_addr) {}

NetAddress::NetAddress(wstring ip, uint16 port)
{
	memset(&_sock_addr, 0, sizeof _sock_addr);
	_sock_addr.sin_family = AF_INET;
	_sock_addr.sin_port = htons(port);
	_sock_addr.sin_addr = IpToAddress(ip.c_str());
}

wstring NetAddress::GetIp()
{
	WCHAR buffer[20];
	InetNtopW(AF_INET, &_sock_addr.sin_addr, buffer, length32(buffer));
	return wstring(buffer);
}

IN_ADDR NetAddress::IpToAddress(const WCHAR* ip)
{
	IN_ADDR address;
	ASSERT_CRASH(InetPtonW(AF_INET, ip, &address) == 1);

	return address;
}
