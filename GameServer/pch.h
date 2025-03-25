#pragma once

#define WIN32_LEAN_AND_MEAN // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

// VC++ 디렉터리 옵션에서 라이브러리 디렉터리 경로에 라이브러리 폴더를 추가해놓음
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