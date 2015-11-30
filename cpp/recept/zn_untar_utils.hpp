#pragma once

#include <string>
#include <map>

class increment_allocator
{
	private:
		int count;

	public:
		increment_allocator():
			count(0)
		{}

		int next()
		{
			return ++count;
		}
};

class directory_allocator
{
	private:
		increment_allocator _ia;
		std::map<std::string, std::string> _directory_map;
		std::string _basename;

	public:
		directory_allocator(const std::string &basename):
			_basename(basename)
		{}

		std::string get_new_directory_name(const std::string &old_directory_name);
};

class tar_filename_processor
{
	private:
		directory_allocator _da;

		std::string _process_directory(const std::string &filename);
		std::string _process_filename(const std::string &filename);

	public:
		tar_filename_processor(const std::string &directory_basename):
			_da(directory_basename)
		{}

		std::string process(const std::string &filename);
};
