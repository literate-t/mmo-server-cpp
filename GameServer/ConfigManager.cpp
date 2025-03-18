#include "pch.h"
#include "ConfigManager.h"
#include "JsonFileManager.h"
#include "nlohmann/json.hpp"

ConfigManager::ConfigManager()
{
	LoadConfig();
}

void ConfigManager::LoadConfig()
{
	string path = "D:/SourceCode/GameSource/MMO_Basic_Cpp/Common/Config/config.json";

	string file_content = JsonFileManager::GetContent(path);

	if (file_content.empty() == false)
		Deserialize(file_content, "dataPath");
}

void ConfigManager::Deserialize(string json_content, string key)
{
	using json = nlohmann::json;

	json j = json::parse(json_content);
	j.at(key).get_to(_data_path);
}
