#pragma once

#include <string>
#include <vector>
#include <memory>
#include <set>
#include <map>

#include "zn_filterable_list.hpp"

struct zn_folder_info
{
	std::string name;
	std::string utf7name;
	int id;

	zn_folder_info():
		id(0)
	{}

	zn_folder_info(int aid, const std::string &aname, const std::string &autf7name):
		name(aname),
		utf7name(autf7name),
		id(aid)
	{}

	std::string serialize();
	bool deserialize(const std::string &data);
};

typedef zn_filter<zn_folder_info> zn_folder_filter;

class zn_folder_unnecessary_folder_remover : public zn_folder_filter
{
	private:
		static bool folder_info_sorter(const zn_folder_info &a, const zn_folder_info &b);

	public:
		virtual bool perform(std::vector<zn_folder_info> &folder_entries);
};

class zn_folder_name_changer : public zn_folder_filter
{
	private:

	public:
		virtual bool perform(std::vector<zn_folder_info> &folder_entries);
};

class zn_folder_priority_sorter : public zn_folder_filter
{
	private:
		std::map<int, int> _mail_counter;

		bool _fill_parent_folder_mail_counter(std::vector<zn_folder_info> &folder_entries);

		bool _remove_empty_default_folder(std::vector<zn_folder_info> &folder_entries);

	public:
		zn_folder_priority_sorter(const std::map<int, int> &mail_counter):
			_mail_counter(mail_counter)
		{}

		virtual bool perform(std::vector<zn_folder_info> &folder_entries);
};

class zn_folder_name_encoder : public zn_folder_filter
{
	private:
		std::set<int> _replacement_folderid;

	public:
		std::set<int> get_replacement_folderid()
		{
			return _replacement_folderid;
		}

		virtual bool perform(std::vector<zn_folder_info> &folder_entries);
};

typedef zn_filterable_list<zn_folder_info> zn_folder_base;

const zn_folder_info &get_folder_info(std::vector<zn_folder_info> &folder_entries, int id);
const zn_folder_info &get_folder_info(std::vector<zn_folder_info> &folder_entries, const std::string &name);

class zn_folder : public zn_folder_base
{
	private:

	public:
		std::string serialize();
		bool deserialize(const std::string &data);
		bool serialize(const std::string &filename);

		const zn_folder_info &get_info(int id)
		{
			return get_folder_info(_entries, id);
		}
		const zn_folder_info &get_info(const std::string &name)
		{
			return get_folder_info(_entries, name);
		}


};

typedef std::auto_ptr<zn_folder> zn_folder_p;
