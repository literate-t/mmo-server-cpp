#include "pch.h"
#include "FileManager.h"
#include <filesystem>
#include <fstream>
#include <sstream>

string FileManager::GetContent(string path)
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

	stringstream buffer;
	buffer << file.rdbuf();
	string file_content = buffer.str();

    return file_content;
}
