#pragma once

class ConfigManager
{
public:
	ConfigManager();
	string GetDataPath() const { return _data_path; }

private:
	void LoadConfig();
	void Deserialize(string& json_content, string key);

private:
	string _data_path;
};

