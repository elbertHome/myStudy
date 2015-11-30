#pragma once

#include <string>
#include <sstream>
#include <exception>

#define ZN_THROW(exception_class, args...)                     \
	do                                                         \
	{                                                          \
		exception_class e(args);                               \
		e.info(__FILE__, __PRETTY_FUNCTION__, __LINE__);       \
		throw e;                                               \
	}                                                          \
	while (false)     
				

class zn_exception : public std::exception
{
	private:
		std::string _what;
		const char *_file;
		const char *_func;
		int _line;

	public:
		explicit zn_exception(const std::string& what_arg):
				_what(what_arg),
				_file(NULL),
				_func(NULL),
				_line(0)
		{}

		virtual ~zn_exception() throw ()
		{}

		virtual const char* what() const throw()
		{
			std::ostringstream oss;
			oss << "file: ";
			oss << _file;
			oss << ", ";
			oss << "func: ";
			oss << _func;
			oss << ", ";
			oss << "line: ";
			oss << _line;
			oss << ":: ";
			oss << _what;

			return oss.str().c_str();
		}

		virtual void what(const std::string &str)
		{
			_what = str;
		}

		void info(const char* file, const char* func, int line)
		{
			_file = file;
			_func = func;
			_line = line;
		}
};

class zn_retry_exception : public zn_exception
{
	public:
		explicit zn_retry_exception(const std::string &what_arg):
			zn_exception(what_arg)
		{}
};

class zn_retry_exceed_exception : public zn_exception
{
	public:
		explicit zn_retry_exceed_exception(const std::string &what_arg):
			zn_exception(what_arg)
		{}
};

class zn_http_exception : public zn_exception
{
	public:
		explicit zn_http_exception(const std::string& what_arg):
			zn_exception(what_arg)
		{}
};

class zn_http_common_exception : public zn_http_exception
{
	public:
		explicit zn_http_common_exception(const std::string &what_arg):
			zn_http_exception(what_arg)
		{}
};

class zn_http_transfer_exception : public zn_http_exception
{
	private:
		int _errcode;

	public:
		explicit zn_http_transfer_exception(int error_code):
			zn_http_exception(""),
			_errcode(error_code)
		{
			std::ostringstream oss;
			oss << "Error code: ";
			oss << error_code;

			zn_http_exception(oss.str());
		}

		int errcode() const
		{
			return _errcode;
		}
};

class zn_xml_exception : public zn_exception
{
	public:
		explicit zn_xml_exception(const std::string& what_arg):
			zn_exception(what_arg)
		{}

		explicit zn_xml_exception(unsigned int err_line, const std::string& err_string):
			zn_exception("")
		{
			std::ostringstream oss;
			oss << "Parse error at line: ";
			oss << err_line;
			oss << ", ";
			oss << "Error String: ";
			oss << err_string;

			this->what(oss.str());
		}
};

class zn_digest_exception : public zn_exception
{
	public:
		explicit zn_digest_exception(const std::string &what_arg):
			zn_exception(what_arg)
		{}
};

class zn_untar_exception : public zn_exception
{
	public:
		explicit zn_untar_exception(const std::string &what_arg):
			zn_exception(what_arg)
		{}

		explicit zn_untar_exception(const std::string &filename, const std::string &what_arg):
			zn_exception("")
		{
			std::ostringstream oss;
			oss << what_arg;
			oss << ": ";
			oss << filename;

			this->what(oss.str());
		}
};

class zn_ldap_exception : public zn_exception
{
	public:
		explicit zn_ldap_exception(const std::string &what_arg):
			zn_exception(what_arg)
		{}
};

class zn_ldap_no_account_exception : public zn_ldap_exception
{
	public:
		explicit zn_ldap_no_account_exception(const std::string &what_arg):
			zn_ldap_exception(what_arg)
		{}
};

class zn_ldap_retry_exception : public zn_retry_exception
{
	public:
		explicit zn_ldap_retry_exception(const std::string &what_arg):
			zn_retry_exception(what_arg)
		{}
};

class zn_migration_queue_exception : public zn_exception
{
	public:
		explicit zn_migration_queue_exception(const std::string &what_arg):
			zn_exception(what_arg)
		{}

		explicit zn_migration_queue_exception(const std::string &what_arg, const std::string &sql):
			zn_exception("")
		{
			std::ostringstream oss;
			oss << what_arg;
			oss << " SQL: ";
			oss << sql;

			this->what(oss.str());
		}
};

class zn_cimap_exception : public zn_exception
{
	public:
		explicit zn_cimap_exception(const std::string &what_arg):
			zn_exception(what_arg)
		{}
};

class zn_cimap_connect_failed_exception : public zn_cimap_exception
{
	public:
		explicit zn_cimap_connect_failed_exception(const std::string &what_arg):
			zn_cimap_exception(what_arg)
		{}
};

class zn_cimap_read_exception : public zn_cimap_exception
{
	public:
		explicit zn_cimap_read_exception(const std::string &what_arg):
			zn_cimap_exception(what_arg)
		{}
};

class zn_cimap_write_exception : public zn_cimap_exception
{
	public:
		explicit zn_cimap_write_exception(const std::string &what_arg):
			zn_cimap_exception(what_arg)
		{}
};

class zn_cimap_no_exception : public zn_cimap_exception
{
	public:
		explicit zn_cimap_no_exception(const std::string &what_arg):
			zn_cimap_exception(what_arg)
		{}
};

class zn_cimap_bad_exception : public zn_cimap_exception
{
	public:
		explicit zn_cimap_bad_exception(const std::string &what_arg):
			zn_cimap_exception(what_arg)
		{}
};

class zn_cimap_response_error_exception : public zn_cimap_exception
{
	public:
		explicit zn_cimap_response_error_exception(const std::string &what_arg):
			zn_cimap_exception(what_arg)
		{}
};

class zn_exclusive_lock_exception : public zn_exception
{
	public:
		explicit zn_exclusive_lock_exception(const std::string &what_arg):
			zn_exception(what_arg)
		{}
};

class zn_exclusive_lock_timeout_exception : public zn_exclusive_lock_exception
{
	public:
		explicit zn_exclusive_lock_timeout_exception(const std::string &what_arg):
			zn_exclusive_lock_exception(what_arg)
		{}
};

class zn_syserr_exception : public zn_exception
{
	public:
		explicit zn_syserr_exception(const std::string &what_arg):
			zn_exception(what_arg)
		{}
};

class zn_folder_name_encoder_exception : public zn_exception
{
	public:
		explicit zn_folder_name_encoder_exception(const std::string &what_arg):
			zn_exception(what_arg)
		{}
};

class zn_folder_name_convert_exception : public zn_folder_name_encoder_exception
{
	public:
		explicit zn_folder_name_convert_exception(const std::string& what_arg):
			zn_folder_name_encoder_exception(what_arg)
		{}

		explicit zn_folder_name_convert_exception(unsigned int err_code, const char* err_string):
			zn_folder_name_encoder_exception("")
		{
			std::ostringstream oss;
			oss << "Convert UTF-7 error. Error Num: ";
			oss << err_code;
			oss << ", ";
			oss << "Error String: ";
			oss << err_string;

			this->what(oss.str());
		}
};

class zn_folder_name_length_exception : public zn_folder_name_encoder_exception
{
	public:
		explicit zn_folder_name_length_exception(const std::string& what_arg):
			zn_folder_name_encoder_exception(what_arg)
		{}

		explicit zn_folder_name_length_exception(const char* folder_path):
			zn_folder_name_encoder_exception("")
		{
			std::ostringstream oss;
			oss << "Folder path is larger than 248Byte. Folder path = ";
			oss << folder_path;

			this->what(oss.str());
		}

		explicit zn_folder_name_length_exception(const char* folder_path, const char* folder_name):
				zn_folder_name_encoder_exception("")
		{
			std::ostringstream oss;
			oss << "Folder name is larger than 64Byte. Folder path = ";
			oss << folder_path;
			oss << ", Folder name = ";
			oss << folder_name;

			this->what(oss.str());
		}
};

class zn_folder_info_exception : public zn_exception
{
	public:
		explicit zn_folder_info_exception(const std::string &what_arg):
			zn_exception(what_arg)
		{}
};

class zn_logger_exception : public zn_exception
{
	public:
		explicit zn_logger_exception(const std::string &what_arg):
			zn_exception(what_arg)
		{}

		explicit zn_logger_exception(const std::string &what_arg, const std::string& msg_param):
					zn_exception("")
		{
			std::ostringstream oss;
			oss << what_arg;
			oss << msg_param;
			this->what(oss.str());
		}
};
