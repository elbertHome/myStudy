#pragma once

#include <string>
#include "expat.h"

#include "zn_folder_info.hpp"
#include "zn_exception.hpp"

class zn_xml
{
	private:
		static void _parse_xml(void* user_data, const std::string &xml, XML_StartElementHandler start_handler,
							XML_EndElementHandler end_handler, XML_CharacterDataHandler character_handler);
		static std::string _get_localconfig(const std::string &xml, std::string const &key_name);
		static std::string _get_element_value(const std::string &xml, const std::vector<std::string>  &tag_list);

	public:
		static std::string get_element(const std::string &xml, std::string const &element_name);
		static zn_folder_p get_zimbra_folder(const std::string &xml);
		static std::string get_zimbra_mysql_passwd(const std::string &xml);
		static std::string check_soap_error(const std::string &xml);
};
