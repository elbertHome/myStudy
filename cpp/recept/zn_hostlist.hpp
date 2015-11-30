#pragma once

#include <map>
#include <string>

typedef std::map<std::string, std::string> zn_hosts;

class zn_hostlist
{
	private:
		zn_hosts _hostlist;
		bool _parse_hostlist(std::string file_line, std::pair<std::string, std::string>& host);

	public:
		std::string get_ip(const std::string &parameter_name);
		bool load(const std::string &list_filename);
};

typedef zn_hostlist mbs_hostlist;
typedef zn_hostlist nsim_hostlist;
