#pragma once

#include <string>

#include "zn_misc.hpp"

// this class is not thread safe!!
class auto_recycle_file
{
	private:
		int *_recycle_counter;
		std::string _filename;

	public:
		// constructor
		auto_recycle_file():
			_recycle_counter(new int)
		{
			*_recycle_counter = 0;
		}
		// copy constructor
		auto_recycle_file(const auto_recycle_file& file):
			_recycle_counter(file._recycle_counter),
			_filename(file._filename)
		{
			counter_plus();
		}
		// move constructor
		/*
		auto_recycle_file& operator=(const auto_recycle_file& file)
		{
			this->_filename = file._filename;
			counter_plus();
			return *this;
		}
		*/
		~auto_recycle_file()
		{
			counter_minus();
			if (_filename.length() != 0 && ((*_recycle_counter) == 0))
			{
				//remove_with_directory(_filename.c_str());
				delete _recycle_counter;
			}
		}
		void bind(std::string &filename)
		{
			_filename = filename;
			counter_plus();
		}
		void counter_plus()
		{
			if (_filename.length() != 0)
				(*_recycle_counter)++;
		}
		void counter_minus()
		{
			if (_filename.length() != 0)
				(*_recycle_counter)--;
		}
};

class zn_mailfile : public auto_recycle_file
{
	private:
		std::string _filename;

	public:
		void bind_file(std::string &filename);

		bool is_available();
		std::string get_content();

		std::string serialize();
		bool deserialize(const std::string &data);
};
