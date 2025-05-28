#pragma once

class Session;

// ------ EventType ------ //
enum class EventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	//PreRecv, zero btye recv
	Recv,
	Send
};

// ------ IocpEvent ------ //
class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void Init();
	EventType GetType() { return _type; }

public:
	SharedIocpObject owner;

public:
	EventType _type;
};

// ------ ConnectEvent ------ //
class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) {}
};

// ------ DisconnectEvent ------ //
class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::Disconnect) {}
};

// ------ AcceptEvent ------ //
class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) {}

public:
	SharedSession session;
};

// ------ RecvEvent ------ //
class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) {}
};

// ------ SendEvent ------ //
class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) {}

	xvector<SharedSendBuffer> send_buffers;
};