#include <cassert>
#include <cstdlib>
#include <cstring>

#include "zn_xml.hpp"
#include "zn_misc.hpp"

void zn_xml::_parse_xml(void* user_data, const std::string &xml, XML_StartElementHandler start_handler,
								XML_EndElementHandler end_handler, XML_CharacterDataHandler character_handler)
{
	XML_Parser parser = XML_ParserCreate(NULL);
    if (!parser)
    {
	    ZN_THROW(zn_xml_exception, "Create parser error.");
	}

	XML_SetUserData(parser, user_data);
	XML_SetElementHandler(parser, start_handler, end_handler);
	if(character_handler != NULL)
	{
		XML_SetCharacterDataHandler(parser, character_handler);
	}

	const int xml_final_flag = 1;
	if (!XML_Parse(parser, xml.c_str(), xml.length(), xml_final_flag))
	{
		XML_ParserFree(parser);
		ZN_THROW(zn_xml_exception,
				XML_GetCurrentLineNumber(parser),
				XML_ErrorString(XML_GetErrorCode(parser)));
	}

	XML_ParserFree(parser);
}

// for get folder
struct zn_xml_folderdata
{
	zn_folder_p folder;
	std::string current_tag;

	zn_xml_folderdata():
		folder(new zn_folder)
	{}
};

static void XMLCALL folder_element_start(void *userdata, const char *el, const char **attr)
{
	if (strcmp("folder", el) == 0)
	{
		zn_xml_folderdata *p_folderdata = reinterpret_cast<zn_xml_folderdata *>(userdata);
		zn_folder_info folder_info;
		for (int i = 0; attr[i]; i += 2)
		{
			if (strcmp("name", attr[i]) == 0)
			{
				if (strcmp(attr[i + 1], "USER_ROOT") == 0)
				{
					return;
				}

				p_folderdata->current_tag += "/";
				p_folderdata->current_tag += attr[i + 1];

				folder_info.name = p_folderdata->current_tag;
			}
			else if (strcmp("i4ms", attr[i]) == 0)
			{
				//folder_info.i4ms = atoi(attr[i + 1]);
			}
			else if (strcmp("i4next", attr[i]) == 0)
			{
				//folder_info.i4next = atoi(attr[i + 1]);
			}
			else if (strcmp("id", attr[i]) == 0)
			{
				folder_info.id = atoi(attr[i + 1]);
			}
			else if (strcmp("l", attr[i]) == 0)
			{
				//folder_info.l = atoi(attr[i + 1]);
			}
			else if (strcmp("ms", attr[i]) == 0)
			{
				//folder_info.ms = atoi(attr[i + 1]);
			}
			else if (strcmp("n", attr[i]) == 0)
			{
				//folder_info.n = atoi(attr[i + 1]);
			}
			else if (strcmp("rev", attr[i]) == 0)
			{
				//folder_info.rev = atoi(attr[i + 1]);
			}
			else if (strcmp("s", attr[i]) == 0)
			{
				//folder_info.s = atoi(attr[i + 1]);
			}
		}

		p_folderdata->folder->add_entry(folder_info);
	}
}

static void XMLCALL folder_element_end(void *userdata, const char *el)
{
	if (strcmp("folder", el) == 0)
	{
		zn_xml_folderdata *p_folderdata = reinterpret_cast<zn_xml_folderdata *>(userdata);
		p_folderdata->current_tag = dirname(p_folderdata->current_tag);
	}
}

zn_folder_p zn_xml::get_zimbra_folder(const std::string &xml)
{
	zn_xml_folderdata userdata;
	zn_xml::_parse_xml(&userdata, xml, folder_element_start, folder_element_end, NULL);

	return userdata.folder;
}

// get zimbra local config
struct zn_xml_localconfig
{
	std::string target_key_name;
	std::string element_value;

	bool target_key_flag;
	bool target_value_flag;

	zn_xml_localconfig():
		 target_key_flag(false),target_value_flag(false)
	{}
};

static void XMLCALL get_localconfig_start(void *userdata, const char *el, const char **attr)
{
	zn_xml_localconfig *p_userdata = reinterpret_cast<zn_xml_localconfig *>(userdata);
	if (strcmp(el, "key") == 0)
	{
		if(p_userdata->target_key_name.compare(attr[1]) == 0)
		{
			// entered target key
			p_userdata->target_key_flag = true;
		}
	}
	else if (p_userdata->target_key_flag == true
			&& strcmp(el, "value") == 0)
	{
		// entered target value
		p_userdata->target_value_flag = true;
	}
}

static void XMLCALL get_localconfig_end(void *userdata, const char *el)
{
	zn_xml_localconfig *p_userdata = reinterpret_cast<zn_xml_localconfig *>(userdata);
	if (p_userdata->target_key_flag
		|| p_userdata->target_value_flag)
	{
		// leave target value
		p_userdata->target_key_flag = false;
		p_userdata->target_value_flag = false;
	}
}

static void XMLCALL get_localconfig_value_handler(void *userdata, const XML_Char *s, int len)
{
	zn_xml_localconfig *p_userdata = (zn_xml_localconfig *)userdata;
	if (p_userdata->target_value_flag)
	{
		assert(p_userdata->element_value.length() == 0);
		p_userdata->element_value.append(s, len);
	}
}

std::string zn_xml::_get_localconfig(const std::string &xml, std::string const &key_name)
{
	zn_xml_localconfig userdata;
	userdata.target_key_name = key_name;
	zn_xml::_parse_xml(&userdata, xml, get_localconfig_start, get_localconfig_end, get_localconfig_value_handler);

	return userdata.element_value;
}

struct zn_xml_element
{
	std::vector<std::string> tag_list;
	std::string element_value;
	size_t tag_index;
	bool target_value_flag;

	zn_xml_element():
		tag_index(0),target_value_flag(false)
	{}
};

static void XMLCALL get_element_start(void *userdata, const char *el, const char **attr)
{
	zn_xml_element *p_userdata = reinterpret_cast<zn_xml_element *>(userdata);
	if(p_userdata->tag_list[p_userdata->tag_index].compare(el) == 0)
	{
		// entered target key
		p_userdata->tag_index++;
	}
	if(p_userdata->tag_index == p_userdata->tag_list.size())
	{
		p_userdata->target_value_flag = true;
		p_userdata->tag_index = p_userdata->tag_list.size() - 1;
	}
}

static void XMLCALL get_element_value_handler(void *userdata, const XML_Char *s, int len)
{
	zn_xml_element *p_userdata = (zn_xml_element *)userdata;
	if (p_userdata->target_value_flag)
	{
		assert(p_userdata->element_value.length() == 0);
		p_userdata->element_value.append(s, len);
		p_userdata->target_value_flag = false;
	}
}
static void XMLCALL get_element_end(void *userdata, const char *el)
{
	zn_xml_element *p_userdata = reinterpret_cast<zn_xml_element *>(userdata);
	if(p_userdata->tag_index == 0)
	{
		return;
	}
	if(p_userdata->tag_list[p_userdata->tag_index - 1].compare(el) == 0)
	{
		// entered target key
		p_userdata->tag_index--;
	}
}
std::string zn_xml::get_element(const std::string &xml, std::string const &element_name)
{

	std::vector<std::string> tag_list;
	tag_list.push_back(element_name);
	return zn_xml::_get_element_value(xml, tag_list);
}

std::string zn_xml::_get_element_value(const std::string &xml, const std::vector<std::string>  &tag_list)
{
	zn_xml_element userdata;
	userdata.tag_list = tag_list;
	zn_xml::_parse_xml(&userdata, xml, get_element_start,get_element_end, get_element_value_handler);

	return userdata.element_value;
}


std::string zn_xml::get_zimbra_mysql_passwd(const std::string &xml)
{
	return zn_xml::_get_localconfig(xml, "zimbra_mysql_password");
}

std::string zn_xml::check_soap_error(const std::string &xml)
{
	std::vector<std::string> tag_list;
	tag_list.push_back("soap:Fault");
	tag_list.push_back("soap:Reason");
	tag_list.push_back("soap:Text");
	return zn_xml::_get_element_value(xml, tag_list);
}

/*
int main()
{
	std::string xml = readfile("b.xml");
	try{
		zn_folder f = zn_xml::get_folders(xml);
		for (std::vector<zn_folder_info>::iterator it = f->begin(); it != f->end(); ++it)
		{
			std::cout << it->name << ": ";
			std::cout << it->i4ms << ", ";
			std::cout << it->i4next << ", ";
			std::cout << it->id << ", ";
			std::cout << it->l << ", ";
			std::cout << it->ms << ", ";
			std::cout << it->n << ", ";
			std::cout << it->rev << ", ";
			std::cout << it->s << std::endl;
		}
	}
	catch(const zn_xml_exception &e)
	{
		std::cout << e.what() << std::endl;

	}
}

*/
