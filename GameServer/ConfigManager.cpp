#include "pch.h"
#include "ConfigManager.h"
#include "FileManager.h"
#include <fstream>
#include <sstream>

ConfigManager::ConfigManager()
{
	LoadConfig();
	}

void ConfigManager::LoadConfig()
	{
	string path = "D:/SourceCode/GameSource/MMO_Basic_Cpp/Common/Config/config.json";

	string file_content = FileManager::GetContent(path);

	if (file_content.empty() == false)
		Deserialize(file_content, "dataPath");
}

string ConfigManager::Deserialize(string key)
{
	//// TODO
	// example
	//using json = nlohmann::json;

	//struct Person {
	//	std::string name;
	//	int age;
	//};

	//// 사용자 정의 타입 Person에 대한 from_json 함수 정의
	//void from_json(const json & j, Person & p) {
	//	j.at("name").get_to(p.name);
	//	j.at("age").get_to(p.age);
	//}
	
	// std::vector<Person> persons = j.at("persons").get<std::vector<Person>>();
	// Person person = j.get<Person>();
}

string ConfigManager::GetDataPath()
{
	return _data_path;
}
