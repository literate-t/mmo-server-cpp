#include "pch.h"
#include "ConfigManager.h"
#include <filesystem>
#include <fstream>
#include <sstream>

void ConfigManager::LoadConfig()
{
	filesystem::path file_path = "D:/SourceCode/GameSource/MMO_Basic_Cpp/Common/Config/config.json";

	if (!filesystem::exists(file_path))
	{
		cerr << "No file: " << file_path << "\n";
		return;
	}

	ifstream file(file_path);
	if (!file)
	{
		std::cerr << "File not open: " << file_path << "\n";
		return;
	}

	stringstream buffer;
	buffer << file.rdbuf();
	string file_content = buffer.str();

	_data_path = Deserialize("dataPath");
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
