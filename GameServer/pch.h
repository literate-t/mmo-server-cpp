#pragma once

#define WIN32_LEAN_AND_MEAN // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

// VC++ ���͸� �ɼǿ��� ���̺귯�� ���͸� ��ο� ���̺귯�� ������ �߰��س���
#ifdef _DEBUG
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif

#include "CorePch.h"
#include "Types.h"
#include "Protocol.pb.h"

// Global
extern class ConfigManager* g_config_manager;
extern class DataManager* g_data_manager;
extern class RoomManager* g_room_manager;