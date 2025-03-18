#include "pch.h"
#include "JsonFileManager.h"
#include <filesystem>
#include <fstream>
#include <sstream>

string JsonFileManager::GetContent(string path)
{
    filesystem::path file_path = path;
	if (!filesystem::exists(file_path))
	{
		cerr << "No file: " << file_path << "\n";
		return string();
	}

	ifstream file(file_path);
	if (!file)
	{
		std::cerr << "File not open: " << file_path << "\n";
		return string();
	}

	stringstream wss; 
	wss << file.rdbuf();

    return wss.str();
}
