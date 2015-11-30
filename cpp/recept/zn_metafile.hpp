#pragma once

#include <map>
#include <string>

#include "zn_mailfile.hpp"

class zn_metafile : public auto_recycle_file
{
    private:
		std::map<std::string, std::string> _meta;

    public:
        bool load_meta(const std::string &content);
		std::string get_value(const std::string &key) const;
		bool set_value(const std::string &key, const std::string &value);
		std::string dump();

		bool is_read;
		std::string date;
		int id;
		int folder_id;

		zn_metafile():
			is_read(false),
			id(0),
			folder_id(0)
		{}

		std::string serialize();
		bool deserialize(const std::string &data);
};
