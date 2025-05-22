#include "pch.h"
#include "ConfigManager.h"
#include "DataManager.h"
#include "RoomManager.h"
#include "ObjectManager.h"
#include "PacketManager.h"
#include "Map.h"
#include "Heartbeat.h"

ConfigManager* g_config_manager = nullptr;
DataManager* g_data_manager = nullptr;
RoomManager* g_room_manager = nullptr;
ObjectManager* g_object_manager = nullptr;
shared_ptr<PacketManager> g_shared_packet_manager = nullptr;
Map* g_map = nullptr;
shared_ptr<Heartbeat> g_shared_heart = nullptr;

class InitGlobal
{
public:
	InitGlobal()
	{
		g_config_manager = new ConfigManager();
		g_data_manager = new DataManager(g_config_manager);
		g_room_manager = new RoomManager();
		g_object_manager = new ObjectManager();
		g_shared_packet_manager = MakeShared<PacketManager>();
		g_map = new Map();
		g_shared_heart = MakeShared<Heartbeat>();
	}

	~InitGlobal()
	{
		delete g_data_manager;
		delete g_config_manager;
		delete g_room_manager;
		delete g_object_manager;
		g_shared_packet_manager = nullptr;
		delete g_map;
		g_shared_heart = nullptr;
	}
};

InitGlobal init_global;

// 컴파일러 최적화로 인해 전역 변수나 init_global 객체가 실제로 사용되지 않으면
// 아예 생성이 안 될 수 있음
void ForceUseInitGlobal()
{
	init_global;
}