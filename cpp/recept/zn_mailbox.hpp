#pragma once

#include <vector>
#include <string>
#include <memory>
#include <set>
#include <map>

#include "zn_metafile.hpp"
#include "zn_mailfile.hpp"

#include "zn_filterable_list.hpp"

struct zn_mailbox_entry
{
	zn_metafile meta;
	zn_mailfile mail;
	bool avaliable;

	zn_mailbox_entry():
		avaliable(true)
	{}

	std::string serialize();
	bool deserialize(const std::string &data);
};

typedef zn_filter<zn_mailbox_entry> zn_mailbox_filter;

class zn_mailbox_limiter : public zn_mailbox_filter
{
	private:
		static bool compare_func(const zn_mailbox_entry &a, const zn_mailbox_entry &b);
		size_t _limit;

	public:
		zn_mailbox_limiter(size_t limit):
			_limit(limit)
		{}
		virtual bool perform(std::vector<zn_mailbox_entry> &mailbox_entries);
};

class zn_mailbox_sorter : public zn_mailbox_filter
{
	private:
		static bool compare_func(const zn_mailbox_entry &a, const zn_mailbox_entry &b);

	public:
		virtual bool perform(std::vector<zn_mailbox_entry> &mailbox_entries);
};

class zn_mailbox_backup_folder_replacement : public zn_mailbox_filter
{
	private:
		std::set<int> _replacement_folderid;

	public:
		zn_mailbox_backup_folder_replacement(const std::set<int> &replacement_folderid):
			_replacement_folderid(replacement_folderid)
		{}

		virtual bool perform(std::vector<zn_mailbox_entry> &mailbox_entries);
};

class zn_mailbox_mail_counter : public zn_mailbox_filter
{
	private:
		std::map<int, int> _mail_counter;

	public:
		std::map<int, int> get_mail_counter()
		{
			return _mail_counter;
		}

		virtual bool perform(std::vector<zn_mailbox_entry> &mailbox_entries);
};

typedef zn_filterable_list<zn_mailbox_entry> zn_mailbox_base;

class zn_mailbox : public zn_mailbox_base
{
	private:

	public:
		std::string serialize();
		bool deserialize(const std::string &data);
		bool serialize(const std::string &filename);
};

typedef std::auto_ptr<zn_mailbox> zn_mailbox_p;
