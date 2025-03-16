#pragma once

class ConfigManager
{
private:
	static string _data_path;

private:
	static string Deserialize(string key);

public:
	static void LoadConfig();
	string GetDataPath();
};

