#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

#include "zn_folder_info.hpp"
#include "zn_utf7_convert.hpp"

#include "zn_misc.hpp"

#include "zn_exception.hpp"
#include "zn_retriable.hpp"
#include "zn_logger.hpp"
#include "zn_config_manager.hpp"

// zn_folder_info
std::string zn_folder_info::serialize()
{
	std::string ret;

	ret += to_string(id);
	ret += ",";
	ret += name;

	ret += ",";
	ret += utf7name;

	return ret;
}

bool zn_folder_info::deserialize(const std::string &data)
{
	std::vector<std::string> v = split(data, ',');
	id = from_string<int>(v[0]);
	name = v[1];

	return true;
}

// zn_folder_unnecessary_folder_remover
static int str2int(const std::string &str)
{
	return from_string<int>(str);
}

bool zn_folder_unnecessary_folder_remover::folder_info_sorter(const zn_folder_info &a, const zn_folder_info &b)
{
	return a.id < b.id;
}

/*
class unnecessary_folder_checker
{
	private:
		std::set<int> &_default_id_set;
		int &_folder_custom_id;

	public:
		unnecessary_folder_checker(std::set<int> &default_id_set, int &folder_custom_id):
			_default_id_set(default_id_set),
			_folder_custom_id(folder_custom_id)
		{}
		bool operator()(const zn_folder_info &info)
		{
			return ((info.id < _folder_custom_id) &&
					(_default_id_set.find(info.id) == _default_id_set.end()));
		}
};
*/

// zn_folder_unnecessary_folder_remover
bool zn_folder_unnecessary_folder_remover::perform(std::vector<zn_folder_info> &folder_entries)
{
	// デフォルトフォルダ：　folder id=2 (Inbox)、4(Junk)、5(Sent)、6(Drafts)の行
	std::string folder_default_id = zn_config_manager::instance().get("FOLDER_DEFAULT_ID");
	std::vector<std::string> str_ids = split(folder_default_id, ',');

	std::vector<int> int_ids(str_ids.size());
	std::transform(str_ids.begin(), str_ids.end(), int_ids.begin(), str2int);

	std::set<int> default_id_set(int_ids.begin(), int_ids.end());

	// ユーザカスタムフォルダ：　folder id >=257の行
	int folder_custom_id = zn_config_manager::instance().get_long("FOLDER_CUSTOM_ID");

	// filt
	/*
	std::vector<zn_folder_info>::iterator it_end = remove_if(folder_entries.begin(), folder_entries.end(), unnecessary_folder_checker(default_id_set, folder_custom_id));
	folder_entries.erase(it_end, folder_entries.end());
	*/
	for (std::vector<zn_folder_info>::iterator it = folder_entries.begin();
			it != folder_entries.end();)
	{
		if ((it->id < folder_custom_id) &&
				(default_id_set.find(it->id) == default_id_set.end()))
		{
			it = folder_entries.erase(it);
		}
		else
		{
			++it;
		}
	}

	// sort
	std::sort(folder_entries.begin(), folder_entries.end(), folder_info_sorter);

	return true;
}

// zn_folder_name_changer
bool zn_folder_name_changer::perform(std::vector<zn_folder_info> &folder_entries)
{
	// 	5=Sent	→　5=Sent Messages
	static std::string from_str = "/Sent";
	static std::string to_str = "/Sent Messages";

	static std::string from_str_sub = "/Sent/";
	static std::string to_str_sub = "/Sent Messages/";

	for (std::vector<zn_folder_info>::iterator it = folder_entries.begin();
			it != folder_entries.end(); ++it)
	{
		// no sub folder.
		if (it->name.compare(from_str) == 0)
		{
			it->name = to_str;
		}

		// have sub folder.
		if (it->name.compare(0, from_str_sub.length(), from_str_sub) == 0)
		{
			it->name.replace(0, from_str_sub.length(), to_str_sub);
		}
	}

	return true;
}

// zn_folder_name_encoder
bool zn_folder_name_encoder::perform(std::vector<zn_folder_info> &folder_entries)
{
	// 	5=Sent	→　5=Sent Messages
	for (std::vector<zn_folder_info>::iterator it = folder_entries.begin();
			it != folder_entries.end();)
	{
		try
		{
			it->utf7name = zn_utf7_converter::zn_convert_folder_path(it->name);
		}
		catch (zn_folder_name_encoder_exception &e)
		{
			int nsim_backup_folder_id = zn_config_manager::instance().get_long("NSIM_BACKUP_FOLDER_ID");
			if (folder_entries.back().id != nsim_backup_folder_id)
			{
				std::string nsim_backup_folder_name = "/";
				nsim_backup_folder_name += zn_config_manager::instance().get("NSIM_BACKUP_FOLDER_NAME");
				std::string nsim_backup_folder_utf7name = zn_utf7_converter::zn_convert_folder_path(nsim_backup_folder_name);
				folder_entries.push_back(zn_folder_info(nsim_backup_folder_id, nsim_backup_folder_name, nsim_backup_folder_utf7name));
			}

			_replacement_folderid.insert(it->id);

			it = folder_entries.erase(it);
			continue;
		}

		++it;
	}

	return true;
}

// zn_folder_priority_sorter
static int get_folder_level(const std::string &foldername)
{
	int ret = 0;

	for (std::string::const_iterator it = foldername.begin(); it != foldername.end(); ++it)
	{
		if (*it == '/')
		{
			ret++;
		}
	}

	return ret;
}

class folder_info_priority_cmp
{
	private:
		std::map<int, int> _mail_counter;

	public:
		folder_info_priority_cmp(const std::map<int, int> &mail_counter):
			_mail_counter(mail_counter)
		{}
		bool operator()(const zn_folder_info &a, const zn_folder_info &b)
		{
			// 移行対象のメールが格納されているフォルダ（親を含む）を優先
			bool mail_exist = _mail_counter.count(a.id) > _mail_counter.count(b.id);
			// 構想構造がrootディレクトリに近いフォルダを優先
			bool folder_level = get_folder_level(a.name) < get_folder_level(b.name);
			// ItemID昇順で優先
			bool id_ascending = a.id < b.id;

			// 代替フォルダは優先度を最低（フォルダ部の最下部）とする
			int nsim_backup_folder_id = zn_config_manager::instance().get_long("NSIM_BACKUP_FOLDER_ID");
			bool replacement_flag = (a.id == nsim_backup_folder_id)?false:true;

			return replacement_flag && (mail_exist || folder_level || id_ascending);
		}
};

class find_folder_info
{
	private:
		int _id;
		std::string _name;

	public:
		find_folder_info(int id):
			_id(id)
		{}
		find_folder_info(std::string name):
			_id(-1),
			_name(name)
		{}
		bool operator()(const zn_folder_info &info)
		{
			if (_id != -1)
			{
				return _id == info.id;
			}
			else
			{
				return _name == info.name;
			}
		}
};

const zn_folder_info &get_folder_info(std::vector<zn_folder_info> &folder_entries, int id)
{
	std::vector<zn_folder_info>::iterator it = std::find_if(folder_entries.begin(), folder_entries.end(), find_folder_info(id));
	if (it == folder_entries.end())
	{
		ZN_THROW(zn_folder_info_exception, "get folder info by id error.");
	}

	return *it;
}

const zn_folder_info &get_folder_info(std::vector<zn_folder_info> &folder_entries, const std::string &name)
{
	std::vector<zn_folder_info>::iterator it = std::find_if(folder_entries.begin(), folder_entries.end(), find_folder_info(name));
	if (it == folder_entries.end())
	{
		ZN_THROW(zn_folder_info_exception, "get folder info by id error.");
	}

	return *it;
}

bool zn_folder_priority_sorter::_fill_parent_folder_mail_counter(std::vector<zn_folder_info> &folder_entries)
{
	std::map<int, int> parent_mail_counter;

	for (std::map<int, int>::iterator it = _mail_counter.begin(); it != _mail_counter.end(); ++it)
	{
		std::string folder_name = get_folder_info(folder_entries, it->first).name;
		for (std::string dir = dirname(folder_name); dir.length() != 0; dir = dirname(dir))
		{
			int parent_folder_id = get_folder_info(folder_entries, dir).id;
			parent_mail_counter[parent_folder_id] += it->second;
		}
	}

	_mail_counter.insert(parent_mail_counter.begin(), parent_mail_counter.end());

	return true;
}

bool zn_folder_priority_sorter::_remove_empty_default_folder(std::vector<zn_folder_info> &folder_entries)
{
	// デフォルトフォルダ：　folder id=2 (Inbox)、4(Junk)、5(Sent)、6(Drafts)の行
	std::string folder_default_id = zn_config_manager::instance().get("FOLDER_DEFAULT_ID");
	std::vector<std::string> str_ids = split(folder_default_id, ',');

	std::vector<int> int_ids(str_ids.size());
	std::transform(str_ids.begin(), str_ids.end(), int_ids.begin(), str2int);

	std::set<int> default_id_set(int_ids.begin(), int_ids.end());

	for (std::vector<zn_folder_info>::iterator it = folder_entries.begin();
			it != folder_entries.end();)
	{
		if ((default_id_set.find(it->id) != default_id_set.end()) &&
				(_mail_counter.count(it->id) == 0))
		{
			it = folder_entries.erase(it);
		}
		else
		{
			++it;
		}
	}

	return true;
}

bool zn_folder_priority_sorter::perform(std::vector<zn_folder_info> &folder_entries)
{
	// generate mail counter for parent folders
	_fill_parent_folder_mail_counter(folder_entries);

	// (14)	データ移行の優先度でフォルダ情報を並び替える。
	std::sort(folder_entries.begin(), folder_entries.end(), folder_info_priority_cmp(_mail_counter));

	// メールが格納されていないデフォルトフォルダは、作成対象外とするためメモリから破棄する。
	_remove_empty_default_folder(folder_entries);

	return true;
}

// zn_folder
std::string zn_folder::serialize()
{
	std::string ret;

	for (iterator it = begin(); it != end(); ++it)
	{
		ret += it->serialize();
		ret += "\n";
	}

	return ret;
}

bool zn_folder::deserialize(const std::string &data)
{
	static const size_t SERIALIZE_BUF_SIZE = 512;

	std::vector<char> buf(SERIALIZE_BUF_SIZE);
	char *pbuf = &(buf.front());
	std::istringstream iss(data);
	while (iss.good())
	{
		iss.getline(pbuf, SERIALIZE_BUF_SIZE);
		if (pbuf[0] != 0)
		{
			zn_folder_info info;
			info.deserialize(pbuf);
			add_entry(info);
		}
	}

	return true;
}

bool zn_folder::serialize(const std::string &filename)
{
	if (!writefile(filename, serialize()))
	{
		ZN_THROW(zn_folder_info_exception, "serialize error.");
	}

	return true;
}
