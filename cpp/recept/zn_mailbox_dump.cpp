#include <fstream>

#include <cstdio>
#include <cstdlib>

#include "zn_mailbox_dump.hpp"
#include "zn_misc.hpp"

static const std::string META_SUFFIX = ".meta";
static const std::string EML_SUFFIX = ".eml";

static std::string remove_filename_extension(const std::string &filename);

// zn_mailbox_dump_stream
bool zn_mailbox_dump_stream::_init_tmp_data_directory()
{
	char *tempname = tempnam(_data_directory.c_str(), "ZN_");
	if (tempname)
	{
		_data_directory = tempname;
		_data_directory += "/";
	}
	else
	{
		return false;
	}
	free(tempname);

	return true;
}

size_t zn_mailbox_dump_stream::on_write(const zn_tar_info &tar_info, const std::string &content)
{
	zn_tar_info local_tar_info = tar_info;
	std::string filename = _data_directory + local_tar_info.name;
	local_tar_info.name  = filename;
	size_t ret = zn_untar_stream::on_write(local_tar_info, content);

	// calc digest
	_msg_digest.feed(content);

	// file donwloaded
	if (tar_info.bytes_left == content.length())
	{

		if (tar_info.name.substr(tar_info.name.length() - META_SUFFIX.length()).compare(META_SUFFIX) == 0)
		{
			zn_mailbox_entry entry;

			std::string content = readfile(filename);
			entry.meta.load_meta(content);
			entry.meta.bind(filename);
			_ap_mailbox->add_entry(entry);
			// TODO!
			// here asume the eml file in same filename is just behind the meta file
			/*
			if (tar_info.gname.compare("folder") == 0)
			{
				_ap_mailbox->push_back(entry);
			}
			else //if (tar_info.gname.compare("message") == 0)
			{
			}
			*/
		}
		else
		{
			std::string digest = _msg_digest.get_digest();
			if (_ap_mailbox->last_entry().meta.get_value("blob_digest").compare(digest) == 0)
			{
				_ap_mailbox->last_entry().mail.bind_file(filename);
			}
			else
			{
				// TODO
				// error for digest not match.
			}
		}
		_msg_digest.clear();
	}

	return ret;
}

std::auto_ptr<zn_mailbox> zn_mailbox_dump_stream::get_mailbox()
{
	return _ap_mailbox;
}

// zn_zimbra_dump
zn_mailbox_p zn_zimbra_mailarchive_dumper::dump(const std::string &mailarchive)
{
	static const size_t MAILFILE_BUF_SIZE = 4096;

	std::string ret;

	std::vector<char> buf(MAILFILE_BUF_SIZE);
	char *pbuf = &(buf.front());
	_dump_stream.set_directory(remove_filename_extension(basename(mailarchive)));
	std::ifstream ifs(mailarchive.c_str(), std::ifstream::in|std::ofstream::binary);
	while (ifs.good())
	{
		ifs.read(pbuf, MAILFILE_BUF_SIZE);

		std::string buf(pbuf, ifs.gcount());
		_dump_stream << (_gunzip_stream << buf);
	}

	return _dump_stream.get_mailbox();
}

// static
std::string remove_filename_extension(const std::string &filename)
{
	size_t ret = filename.find_last_of(".");
	if (ret == std::string::npos)
	{
		return filename;
	}
	else
	{
		return filename.substr(0, ret);
	}
}
