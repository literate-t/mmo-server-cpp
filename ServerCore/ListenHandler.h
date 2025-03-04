#pragma once

#include "IocpCore.h"

class AcceptEvent;
class ServerService;
class ListenHandler : public IocpObject
{
public:
	ListenHandler() = default;
	~ListenHandler();

public:
	bool StartAccept(SharedService server_service);
	void CloseSocket();

public:
	/* interface implementation */
	HANDLE GetHandle() override;
	void Dispatch(class IocpEvent* iocp_event, int32 number_of_bytes = 0) override;

private:
	/* recv */
	void RegisterAccept(AcceptEvent* accept_event);
	void SetupSession(AcceptEvent* accept_event);


protected:
	SOCKET _socket = INVALID_SOCKET;
	xvector<AcceptEvent*> _accept_events;
	// 순환 참조 유의
	// ServerService에서도 ListenHandler를 들고 있음
	SharedService _service;
};
