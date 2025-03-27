#include "pch.h"
#include "StringReader.h"
#include <fstream>

StringReader::StringReader(string data_path)
	:_data_path(data_path), _file(data_path)
{
	if (!_file)
	{
		cerr << "No file" << endl;
		throw runtime_error("File open failed: " + _data_path);
	}
}

string StringReader::ReadLine()
{
	string result;
	getline(_file, result);

	return result;
}
