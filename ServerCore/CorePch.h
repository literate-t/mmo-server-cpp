#pragma once

// CoreMacrh.h�� xalloc ��ũ�� �Լ��� �ְ�
// iostream�� ���� �̸��� �Լ��� �־
// ��ũ�� �Լ� ���� ��Ŭ��� �ϸ� ������ ���� �ʴ´�
#include <iostream>
#include <Windows.h>
#include <utility>
#include <chrono>
#include <optional>
using namespace std;
using namespace chrono_literals;

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "Types.h"
#include "CoreGlobal.h"
#include "CoreTLS.h"
#include "CoreMacro.h"
#include "Container.h"
#include "Lock.h"
#include "ThreadManager.h"
#include "DeadLockDetector.h"
#include "Allocator.h"
#include "Memory.h"
#include "ThreadLocalSlabManager.h"
#include "ObjectPool.h"
#include "SocketUtils.h"
#include "SendBuffer.h"
#include "Session.h"
#include "SessionManager.h"
#include "LockQueue.h"
#include "JobQueue.h"
#include "GlobalQueue.h"
#include "JobTimer.h"
#include "DBConnectionPool.h"
#include "DBBind.h"