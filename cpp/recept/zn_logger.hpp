#pragma once

#include <string>
#include <fstream>
#include "SimpleIni.h"

class zn_log
{
	private:
		std::ofstream &_fs;

		inline std::string _get_datetime();
		inline std::string _get_pid();

	protected:
		void _log_msg(const std::string &level, const std::string &msg);

	public:
		zn_log(std::ofstream &fs):
			_fs(fs)
		{}

		zn_log &operator <<(const std::string &msg)
		{
			return log(msg);
		}

		virtual zn_log &log(const std::string &msg) = 0;
};

class zn_buffered_log : public zn_log
{
	private:
		std::string _buffer;

	protected:
		void _log_msg(const std::string &level, const std::string &msg);

	public:
		static const std::string endl;
		zn_buffered_log(std::ofstream &fs):
			zn_log(fs)
		{}
};

class zn_error_log : public zn_buffered_log
{
	public:
		zn_error_log(std::ofstream &fs):
			zn_buffered_log(fs)
		{}

		virtual zn_log &log(const std::string &msg)
		{
			_log_msg("ERROR", msg);
			return *this;
		}
};

class zn_warning_log : public zn_buffered_log
{
	public:
		zn_warning_log(std::ofstream &fs):
			zn_buffered_log(fs)
		{}

		virtual zn_log &log(const std::string &msg)
		{
			_log_msg("WARN", msg);
			return *this;
		}
};

class zn_info_log : public zn_buffered_log
{
	public:
		zn_info_log(std::ofstream &fs):
			zn_buffered_log(fs)
		{}

		virtual zn_log &log(const std::string &msg)
		{
			_log_msg("INFO", msg);
			return *this;
		}
};

class zn_debug_log : public zn_buffered_log
{
	public:
		zn_debug_log(std::ofstream &fs):
			zn_buffered_log(fs)
		{}

		virtual zn_log &log(const std::string &msg)
		{
			_log_msg("DEBUG", msg);
			return *this;
		}
};

class zn_logger
{
	private:
		const static char *INI_SECTION;

		std::ofstream _ofs;

		zn_error_log _elog;
		zn_warning_log _wlog;
		zn_info_log _ilog;
		zn_debug_log _dlog;

		CSimpleIni _ini;
		std::string _prg_name;
		bool _is_opened;

		zn_logger(const std::string &logfile);
		~zn_logger();

		static zn_logger _instance;

		bool _load_log_msg(const char* message_file);
		std::string _get_log_msg(const char* log_id);

	public:
		static zn_logger& instance();
		static void set_program_name(const std::string& prg_name)
		{
			_instance._prg_name = prg_name;
		}

		void log(const char *log_id, ...);
		void debuglog(const std::string& log);

		zn_log &error()
		{
			return _elog;
		}
		zn_log &warning()
		{
			return _wlog;
		}
		zn_log &info()
		{
			return _ilog;
		}
		zn_log &debug()
		{
			return _dlog;
		}

		void error(const std::string &msg)
		{
			_elog << msg;
		}
		void warning(const std::string &msg)
		{
			_wlog << msg;
		}
		void info(const std::string &msg)
		{
			_ilog << msg;
		}
		void debug(const std::string &msg)
		{
			_dlog << msg;
		}
};
