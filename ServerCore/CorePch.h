// CorePch는 다른 프로젝트에서도 사용할 수 있다
#pragma once

// CoreMacrh.h에 xalloc 매크로 함수가 있고
// iostream에 같은 이름의 함수가 있어서
// 매크로 함수 전에 인클루드 하면 오류가 나지 않는다
#include <iostream>
#include <Windows.h>
#include <utility>
#include <chrono>
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
#include "ObjectPool.h"
#include "TypeCast.h"
#include "SocketUtils.h"
#include "SendBuffer.h"
#include "Session.h"
#include "LockQueue.h"
#include "JobQueue.h"
#include "GlobalQueue.h"
#include "JobTimer.h"
#include "DBConnectionPool.h"
#include "DBBind.h"