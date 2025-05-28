#pragma once

#include "NetAddress.h"
#include "IocpCore.h"
#include <functional>
#include "SessionManager.h"

// ------- ServiceType ------- //
enum class ServiceType : uint8
{
	Server,
	Client
};

using SessionFactory = function<SharedSession(void)>;

// --------- Service --------- //
class Service : public enable_shared_from_this<Service>
{
public:
	// AddressConverter는 서버일 경우 내 주소, 클라일 경우 상대 주소
	Service(ServiceType type, NetAddress address, SharedIocpCore core, SessionFactory fn_session_factory, shared_ptr<SessionManager> session_manger, int32 max_session_count = 1);
	virtual ~Service();

	virtual bool	Start() = 0;
	bool			CanStart() const { return nullptr != _fn_session_factory; }

	virtual void	CloseService() = 0;

	// for chatting test
	void			Broadcast(SharedSendBuffer send_buffer);
	SharedSession	CreateSessionToIocp();
	void			RegisterSession(SharedSession session);
	void			UnregisterSession(SharedSession session);

	void			AddToSessionManager(SharedSession session);
	void			RemoveFromSessionManager(SharedSession session);

	int32			GetCurrentSessionCount() const { return _session_count; }
	int32			GetMaxSessionCount() const { return _max_session_count; }

public:
	ServiceType				GetServiceType() const { return _type; }
	NetAddress				GetNetAddress() const { return _address; }
	const SharedIocpCore&	GetIocpCore() const { return _iocp_core; }
	const SharedSessionManager& GetSharedSessionManager() const{ return _session_manager; }

protected:
	USE_LOCK;
	ServiceType	_type;
	NetAddress _address{};	
	SharedIocpCore _iocp_core;
	SharedSessionManager _session_manager;

	xset<SharedSession> _sessions;
	int32 _session_count;
	int32 _max_session_count;
	SessionFactory _fn_session_factory;
};

// --------- ClientService --------- //
class ClientService : public Service
{
public:
	ClientService(NetAddress target_address, SharedIocpCore iocp_core, SessionFactory fn_session_factory, shared_ptr<SessionManager> session_manger, int32 max_session_count = 1);
	virtual ~ClientService();

	bool Start() override;	
	void CloseService() override;
};

// --------- ServerService --------- //
class ServerService : public Service
{
public:
	ServerService(NetAddress address, SharedIocpCore iocp_core, SessionFactory fn_session_factory, SharedListenHandler listen_handler, shared_ptr<SessionManager> session_manger, int32 max_session_count = 1);
	virtual ~ServerService();

	bool Start() override;
	void CloseService() override;

private:
	// weak_ptr이 가능할까
	SharedListenHandler _listen_handler = nullptr;
};