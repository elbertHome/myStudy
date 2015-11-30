#include <fstream>
#include <vector>

#include <cstdio>

#include "zn_mailfile.hpp"

void zn_mailfile::bind_file(std::string &filename)
{
	_filename = filename;
	auto_recycle_file::bind(filename);
}

bool zn_mailfile::is_available()
{
	std::ifstream ifs(_filename.c_str(), std::ifstream::in);
	return ifs.good();
}

std::string zn_mailfile::get_content()
{
	return readfile(_filename);
}

std::string zn_mailfile::serialize()
{
	return _filename;
}

bool zn_mailfile::deserialize(const std::string &data)
{
	_filename = data;

	return true;
}
