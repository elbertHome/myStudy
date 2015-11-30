#include <algorithm>

#include "zn_mailbox.hpp"

#include "zn_exception.hpp"
#include "zn_logger.hpp"
#include "zn_config_manager.hpp"

// zn_mailbox_entry
std::string zn_mailbox_entry::serialize()
{
	return meta.serialize() + "," + mail.serialize();
}

bool zn_mailbox_entry::deserialize(const std::string &data)
{
	size_t pos = data.find_last_of(',');
	meta.deserialize(data.substr(0, pos));
	mail.deserialize(data.substr(pos + 1));

	return true;
}

// zn_mailbox_limiter
bool zn_mailbox_limiter::compare_func(const zn_mailbox_entry &a, const zn_mailbox_entry &b)
{
	return a.meta.date < b.meta.date;
}

bool zn_mailbox_limiter::perform(std::vector<zn_mailbox_entry> &mailbox_entries)
{
	if (mailbox_entries.size() > _limit)
	{
		std::sort(mailbox_entries.begin(), mailbox_entries.end(), compare_func);
		mailbox_entries.erase(mailbox_entries.begin() + _limit, mailbox_entries.end());
	}

	return true;
}

// zn_mailbox_sorter
bool zn_mailbox_sorter::compare_func(const zn_mailbox_entry &a, const zn_mailbox_entry &b)
{
	return a.meta.id < b.meta.id;
}

bool zn_mailbox_sorter::perform(std::vector<zn_mailbox_entry> &mailbox_entries)
{
	std::sort(mailbox_entries.begin(), mailbox_entries.end(), compare_func);
	return true;
}

// zn_mailbox_backup_folder_replacement
bool zn_mailbox_backup_folder_replacement::perform(std::vector<zn_mailbox_entry> &mailbox_entries)
{
	for (std::vector<zn_mailbox_entry>::iterator it = mailbox_entries.begin();
			it != mailbox_entries.end(); ++it)
	{
		if (_replacement_folderid.count(it->meta.folder_id))
		{
			int nsim_backup_folder_id = zn_config_manager::instance().get_long("NSIM_BACKUP_FOLDER_ID");
			it->meta.folder_id = nsim_backup_folder_id;
		}
	}

	return true;
}

// zn_mailbox_mail_counter
bool zn_mailbox_mail_counter::perform(std::vector<zn_mailbox_entry> &mailbox_entries)
{
	for (std::vector<zn_mailbox_entry>::iterator it = mailbox_entries.begin();
			it != mailbox_entries.end(); ++it)
	{
		_mail_counter[it->meta.folder_id]++;
	}

	return true;
}

// zn_mailbox
std::string zn_mailbox::serialize()
{
	std::string ret;

	for (iterator it = begin(); it != end(); ++it)
	{
		ret += it->serialize();
		ret += "\n";
	}

	return ret;
}

bool zn_mailbox::deserialize(const std::string &data)
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
			zn_mailbox_entry entry;
			entry.deserialize(pbuf);
			add_entry(entry);
		}
	}

	return true;
}

bool zn_mailbox::serialize(const std::string &filename)
{
	return true;
}
