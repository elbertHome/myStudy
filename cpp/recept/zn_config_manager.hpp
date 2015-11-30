#pragma once

#include <string>

#include "SimpleIni.h"

class zn_config_manager
{
	private:
		//zn_config_manager();
		//~zn_config_manager();

		const static char *INI_SECTION;

		CSimpleIni _ini;
		static zn_config_manager _instance;

	public:
		static zn_config_manager &instance()
		{
			return _instance;
		}

		bool load_config_file(const std::string &config_file);

		std::string get(const std::string &parameter_name, const std::string &default_value = "");
		long get_long(const std::string &parameter_name, long default_value = 0);
		bool get_bool(const std::string &parameter_name, bool default_value = false);

		bool set(const std::string &parameter_name, const std::string &value);
};
