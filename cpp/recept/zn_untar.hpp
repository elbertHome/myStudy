#pragma once

#include "zn_stream.hpp"
#include "zn_untar_utils.hpp"

#include "zn_config_manager.hpp"

struct tar_header;

class zn_tar_info
{
	public:
		std::string name;
		size_t size;
		size_t bytes_left;
		time_t mtime;
		std::string gname;

		zn_tar_info():
			size(0),
			bytes_left(0),
			mtime(0)
		{}

		void clear()
		{
			name.clear();
			size = 0;
			bytes_left = 0;
			mtime = 0;
			gname.clear();
		}
};

class zn_untar_stream : public zn_buffered_stream
{
	private:
		static const unsigned int ZN_TAR_BLOCK_SIZE = 512;
		static const int ZN_TAR_END_COUNT = 2;
		enum ZN_TAR_STATUS
		{
			ZN_TAR_HEADER,
			ZN_TAR_CONTENT,
			ZN_TAR_LONGNAME,
			ZN_TAR_LONGNAME_HEADER
		};

		ZN_TAR_STATUS _status;
		int _tar_end_count;

	protected:
		zn_tar_info _tar_info;
		virtual bool _set_tar_info(const tar_header *p_header);

	public:
		zn_untar_stream():
			_status(ZN_TAR_HEADER),
			_tar_end_count(0)
		{}

		bool eof()
		{
			return (_tar_end_count == ZN_TAR_END_COUNT);
		}

		virtual void feed(const std::string &buf);
		virtual size_t on_write(const zn_tar_info &header, const std::string &content);
};

class zn_zimbra_untar_stream : public zn_untar_stream
{
	private:
		tar_filename_processor _tfp;

		virtual bool _set_tar_info(const tar_header *p_header);

	public:
		zn_zimbra_untar_stream():
			zn_untar_stream(),
			_tfp(zn_config_manager::instance().get("FOLDER_RENAME_PREFIX"))
		{}
};
