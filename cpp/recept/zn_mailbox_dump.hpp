#pragma once

#include <string>

#include "zn_untar.hpp"
#include "zn_gunzip.hpp"
#include "zn_mailbox.hpp"
#include "zn_message_digest.hpp"

class zn_mailbox_dump_stream : public zn_zimbra_untar_stream
{
	private:
		std::string _base_directory;
		std::string _data_directory;
		zn_mailbox_p _ap_mailbox;

		zn_message_digest _msg_digest;

		bool _init_tmp_data_directory();

	public:
		zn_mailbox_dump_stream(const std::string &directory):
			_base_directory(directory),
			_data_directory(directory),
			_ap_mailbox(new zn_mailbox)
		{
			//_init_tmp_data_directory();
		}

		virtual size_t on_write(const zn_tar_info &header, const std::string &content);

		zn_mailbox_p get_mailbox();

		void set_directory(const std::string &directory)
		{
			_data_directory = _base_directory + "/" + directory + "/";
		}
};

class zn_zimbra_mailarchive_dumper
{
	private:
		zn_gunzip_stream _gunzip_stream;
		zn_mailbox_dump_stream _dump_stream;

	public:
		zn_zimbra_mailarchive_dumper():
			_dump_stream("/var/tmp")
		{}
		zn_zimbra_mailarchive_dumper(const std::string &temp_directory):
			_dump_stream(temp_directory)
		{}

		std::auto_ptr<zn_mailbox> get_mailbox()
		{
			return _dump_stream.get_mailbox();
		}

		zn_mailbox_p dump(const std::string &mailarchive);
};
