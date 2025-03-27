#pragma once

class StringReader
{
public:
	StringReader(string data_path);
	string ReadLine();

private:
	string _data_path;
	ifstream _file;
};

