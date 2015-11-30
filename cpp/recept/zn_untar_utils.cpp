#include <vector>

#include "zn_untar_utils.hpp"

#include "zn_misc.hpp"

inline bool isdigit(char c);

std::string directory_allocator::get_new_directory_name(const std::string &old_directory_name)
{
	std::string ret;
	std::map<std::string, std::string>::iterator it = _directory_map.find(old_directory_name);
	if (it == _directory_map.end())
	{
		std::string new_name = _basename + to_string(_ia.next());
		_directory_map[old_directory_name] = new_name;
		return new_name;
	}
	else
	{
		return it->second;
	}
}

std::string tar_filename_processor::_process_directory(const std::string &filename)
{
	std::vector<std::string> folders = split(filename, '/');

	std::vector<std::string>::iterator it;
	for (it = folders.begin(); it != folders.end() - 1; ++it)
	{
		*it = _da.get_new_directory_name(*it);
	}

	return join(folders, '/');
}

std::string tar_filename_processor::_process_filename(const std::string &filename)
{
	std::string dname = dirname(filename);
	std::string fname = basename(filename);

	// with email subject
	if (isdigit(fname[0]))
	{
		size_t start = fname.find('-');
		size_t end = fname.find('.');

		if (start != std::string::npos && end != std::string::npos && start < end)
		{
			fname.erase(start, end - start);
		}
	}
	else
	{
		// email without subject won't change name;
	}

	return dname + "/" + fname;
}

std::string tar_filename_processor::process(const std::string &filename)
{
	return _process_directory(_process_filename(filename));
}

bool isdigit(char c)
{
	return '0' <= c && c <= '9';
}
