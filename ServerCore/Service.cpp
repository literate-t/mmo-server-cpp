#include "pch.h"
#include "Service.h"
#include "Session.h"
#include "ListenHandler.h"

Service::Service(ServiceType type, NetAddress address, SharedIocpCore iocp_core, SessionFactory fn_session_factory, SharedSessionManager session_manger, int32 max_session_count)
	: _type(type), _address(address), _iocp_core(iocp_core), _fn_session_factory(fn_session_factory), _max_session_count(max_session_count), _session_manager(session_manger), _session_count(0)
{
}

Service::~Service()
{
}

void Service::Broadcast(SharedSendBuffer send_buffer)
{
	WRITE_LOCK;
	for (const auto& session : _sessions)
	{
		session->Send(send_buffer);
	}
}

SharedSession Service::CreateSessionToIocp()
{
	SharedSession session = _fn_session_factory();
	session->SetService(shared_from_this());

	if (false == _iocp_core->Register(session))
	{
		return nullptr;
	}

	return session;
}

void Service::RegisterSession(SharedSession session)
{
	WRITE_LOCK;
	++_session_count;
	_sessions.insert(session);
}

void Service::UnregisterSession(SharedSession session)
{
	WRITE_LOCK;
	ASSERT_CRASH(0 != _sessions.erase(session));
	--_session_count;
}

void Service::AddToSessionManager(SharedSession session)
{
	_session_manager->Add(session);
}

void Service::RemoveFromSessionManager(SharedSession session)
{
	_session_manager->Remove(session);
}

ClientService::ClientService(NetAddress target_address, SharedIocpCore iocp_core, SessionFactory fn_session_factory, shared_ptr<SessionManager> session_manger, int32 max_session_count)
	: Service(ServiceType::Client, target_address, iocp_core, fn_session_factory, session_manger, max_session_count)
{

}

ClientService::~ClientService()
{
}

bool ClientService::Start()
{
	if (false == CanStart())
		return false;

	const int32 session_count = GetMaxSessionCount();
	for (int32 i = 0; i < session_count; ++i)
	{
		SharedSession session = CreateSessionToIocp();
		if (false == session->Connect())
			return false;
	}

	return true;
}

void ClientService::CloseService()
{

}

ServerService::ServerService(NetAddress address, SharedIocpCore iocp_core, SessionFactory fn_session_factory, SharedListenHandler listen_handler, shared_ptr<SessionManager> session_manger, int32 max_session_count)
	: Service(ServiceType::Server, address, iocp_core, fn_session_factory, session_manger, max_session_count), _listen_handler(listen_handler)
{
}

ServerService::~ServerService()
{
}

bool ServerService::Start()
{
	if (false == CanStart())
		return false;

	SharedServerService service = static_pointer_cast<ServerService>(shared_from_this());

	if (false == _listen_handler->StartAccept(service))
		return false;

	return true;
}

void ServerService::CloseService()
{
}
