#include "pch.h"
#include "ConfigManager.h"
#include "DataManager.h"
#include "RoomManager.h"
#include "ObjectManager.h"
#include "Map.h"

ConfigManager* g_config_manager = nullptr;
DataManager* g_data_manager = nullptr;
RoomManager* g_room_manager = nullptr;
ObjectManager* g_object_manager = nullptr;
Map* g_map = nullptr;

class InitGlobal
{
public:
	InitGlobal()
	{
		g_config_manager = new ConfigManager();
		g_data_manager = new DataManager(g_config_manager);
		g_room_manager = new RoomManager();
		g_object_manager = new ObjectManager();
		g_map = new Map();
	}

	~InitGlobal()
	{
		delete g_data_manager;
		delete g_config_manager;
		delete g_room_manager;
		delete g_object_manager;
		delete g_map;
	}
};

InitGlobal init_global;

// �����Ϸ� ����ȭ�� ���� ���� ������ init_global ��ü�� ������ ������ ������
// �ƿ� ������ �� �� �� ����
void ForceUseInitGlobal()
{
	init_global;
}