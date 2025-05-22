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
#include "Types.h"
#include "Protocol.pb.h"
using namespace Protocol;

// Global
extern class ConfigManager* g_config_manager;
extern class DataManager* g_data_manager;
extern class RoomManager* g_room_manager;
extern class ObjectManager* g_object_manager;
extern shared_ptr<class PacketManager> g_shared_packet_manager;
extern class Map* g_map;
extern shared_ptr<class Heartbeat> g_shared_heart;

// Include
#include <fstream>