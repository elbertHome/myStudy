#include <cstring>
#include <fstream>
#include "zn_hostlist.hpp"
#include "zn_misc.hpp"

static const char SP_CHAR[2 + 1] = " \t";

bool zn_hostlist::_parse_hostlist(std::string file_line, std::pair<std::string, std::string>& host)
{
	size_t pos_start = 0;
	size_t pos_end = 0;
	std::string key;
	std::string value;
	pos_start = file_line.find_first_not_of(SP_CHAR);
	if(pos_start == std::string::npos)
	{
		return false;
	}

	pos_end = file_line.find_first_of(SP_CHAR,pos_start);
	if(pos_end == std::string::npos)
	{
		//no space
		pos_end = file_line.size();
	}

	host.first = file_line.substr(pos_start, pos_end - pos_start);

	pos_start = file_line.find_first_not_of(SP_CHAR, pos_end);
	if(pos_start == std::string::npos)
	{
		return true;
	}
	pos_end = file_line.find_first_of(SP_CHAR,pos_start);
	if(pos_end == std::string::npos)
	{
		pos_end = file_line.size() ;
	}
	host.second = file_line.substr(pos_start, pos_end - pos_start);
	return true;
}


bool zn_hostlist::load(const std::string &list_filename)
{
	static const size_t HOSTLIST_BUF_SIZE = 256;

	char buf[HOSTLIST_BUF_SIZE];
	std::ifstream ifs(list_filename.c_str(), std::ifstream::in|std::ofstream::binary);

	while (ifs.good())
	{
		memset(buf, 0, HOSTLIST_BUF_SIZE);
		std::pair<std::string, std::string> host;
		ifs.getline(buf, HOSTLIST_BUF_SIZE);
		std::string line = buf;
		if(!_parse_hostlist(line, host))
		{
			continue;
		}
		_hostlist[host.first] = host.second;
	}

	if(!_hostlist.size())
	{
		return false;
	}

	return true;
}

std::string zn_hostlist::get_ip(const std::string &parameter_name)
{
	std::map<std::string, std::string>::const_iterator it_find = _hostlist.find(parameter_name);
	if(it_find == _hostlist.end())
	{
		return std::string();
	}
	else
	{
		return it_find->second;
	}
}

//int main()
//{
//	std::string filename = "./hostlist";
//	zn_hostlist hostlist(filename);
//
//	bool ret = hostlist.load_hostlist();
//
//	std::cout << "host:aaa ip:" << hostlist.get_ip("aaa") << std::endl;
//	std::cout << "host:bbb ip:" << hostlist.get_ip("bbb") << std::endl;
//	std::cout << "host:ccc ip:" << hostlist.get_ip("ccc") << std::endl;
//}
