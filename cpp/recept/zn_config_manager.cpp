#include "zn_config_manager.hpp"

const char *zn_config_manager::INI_SECTION = "";

zn_config_manager zn_config_manager::_instance;

bool zn_config_manager::load_config_file(const std::string &config_file)
{
	SI_Error ret =_ini.LoadFile(config_file.c_str());
	return (ret == 0?true:false);
}

std::string zn_config_manager::get(const std::string &parameter_name, const std::string &default_value)
{
	const char *value = _ini.GetValue(INI_SECTION, parameter_name.c_str(), default_value.c_str());
	return std::string(value);
}

long zn_config_manager::get_long(const std::string &parameter_name, long default_value)
{
	return _ini.GetLongValue(INI_SECTION, parameter_name.c_str(), default_value);
}

bool zn_config_manager::get_bool(const std::string &parameter_name, bool default_value)
{
	return _ini.GetBoolValue(INI_SECTION, parameter_name.c_str(), default_value);
}

bool zn_config_manager::set(const std::string &parameter_name, const std::string &value)
{
	SI_Error ret = _ini.SetValue(INI_SECTION, parameter_name.c_str(), value.c_str());
	return (ret == 0?true:false);
}
