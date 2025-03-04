#include "pch.h"
#include "IocpEvent.h"

IocpEvent::IocpEvent(EventType type): _type(type)
{
	Init();
}

void IocpEvent::Init()
{
	Internal = 0;
	InternalHigh = 0;
	Pointer = 0;
	hEvent = nullptr;
}
