#pragma once

#include <string>

class zn_utf7_converter
{
	private:
		static std::string _convert_utf8_to_utf7(std::string in_str);
		static std::string _convert_utf8_to_modified_utf7(const std::string &in_str);

	public:
		static std::string zn_convert_folder_path(const std::string &in_str);
};
