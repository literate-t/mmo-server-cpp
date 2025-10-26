#pragma once

#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
#include "RecvBuffer.h"

// -------- Session -------- //
class Session : public IocpObject
{	
	friend class ListenHandler;

	enum
	{
		BUFFER_SIZE = 0x10000 // 64kB
	};

public:
	Session();
	virtual ~Session();

public:
	void Send(SharedSendBuffer send_buffer);
	void Send(xqueue<SharedSendBuffer>& send_buffers);

	// ���� �� ����� �� �ʿ��� �� �ִ�
	bool Connect();
	void Disconnect(const WCHAR* cause);

	SharedService GetService() { return _service.lock(); }
	void SetService(SharedService service) { _service = service; }

public:
	/* net information */
	void SetAddress(NetAddress address) { _address = address; }
	NetAddress GetAddress() const { return _address; }
	SOCKET GetSocket() const { return _socket; }
	bool IsConnected() const { return _connected; }
	SharedSession GetSharedSession() { return static_pointer_cast<Session>(shared_from_this()); }

	virtual void FlushSend() {}

protected:
	/* ������ �ܿ��� �ʿ��ϸ� �������̵� */
	virtual void OnConnectCompleted() {}
	virtual int32 OnRecvCompleted(BYTE* buffer, int32 length) = 0;
	virtual void OnSendCompleted(int32 length) {}
	virtual void OnDisconnectCompleted() {}


private:
	bool RegisterDisconnect();
	void ProcessDisconnect(IocpEvent* disconnect_event);

	bool RegisterConnect();
	void EstablishConnect(IocpEvent* connect_event);

	void RegisterRecv();
	void ProcessRecv(int32 recv_bytes);

	void RegisterSend();
	void ProcessSend(int32 send_bytes);

	void HandleError(int32 error);

// ������ �ܿ��� ȣ���� ������ ����
private:
	/* interface implementation */
	virtual SOCKET GetSocket() override;
	virtual void Dispatch(class IocpEvent* iocp_event, int32 number_of_bytes = 0) override;

private:
	weak_ptr<class Service> _service;
	SOCKET	_socket{ INVALID_SOCKET };
	NetAddress	_address{};
	Atomic<bool> _connected{ false };

private:
	USE_LOCK;
	/* �ۼ��� ���� */
	RecvBuffer _recv_buffer;
	xqueue<SharedSendBuffer> _send_queue;
	Atomic<bool> _send_registered = false;

private:
	RecvEvent _recv_event;
	SendEvent _send_event;
};

// -------- PacketHeader -------- //
struct PacketHeader
{
	uint16 size;
	uint16 id;
};

// -------- PacketSession -------- //
class PacketSession : public Session
{
public:
	PacketSession();
	virtual ~PacketSession();

	SharedPacketSession GetSharedPacketSession() { return static_pointer_cast<PacketSession>(shared_from_this()); }

protected:
	int32 OnRecvCompleted(BYTE* buffer, int32 length) final;
	virtual void OnRecvPacketProcessed(BYTE* buffer, int32 len) = 0;
};