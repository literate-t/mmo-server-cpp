#pragma once

#define WIN32_LEAN_AND_MEAN // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

// VC++ ���͸� �ɼǿ��� ���̺귯�� ���͸� ��ο� ���̺귯�� ������ �߰��س���
#if defined(MYDEBUG)
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#elif defined(RELEASE)
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif

#include "CorePch.h"
#include "Protocol.pb.h"
using namespace Protocol;

#include <memory>

using SharedServerSession = std::shared_ptr<class ServerSession>;