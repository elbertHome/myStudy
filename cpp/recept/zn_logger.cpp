#include <ctime>

#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <syslog.h>
#include <map>
#include <stdarg.h>

#include "zn_logger.hpp"
#include "zn_misc.hpp"
#include "zn_exception.hpp"
#include "zn_config_manager.hpp"

const char *zn_logger::INI_SECTION = "";
zn_logger zn_logger::_instance("md_msgmove.log");


zn_logger::zn_logger(const std::string &logfile):
	_ofs(logfile.c_str(), std::ofstream::app),
	_elog(_ofs),
	_wlog(_ofs),
	_ilog(_ofs),
	_dlog(_ofs),
	_is_opened(true)
{
	//
}

zn_logger::~zn_logger()
{
	//
}

std::string zn_log::_get_datetime()
{
    time_t now = time(0);
    struct tm tstruct;
    char buf[100];

    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

std::string zn_log::_get_pid()
{
	return to_string(getpid());
}

void zn_log::_log_msg(const std::string &level, const std::string &msg)
{
	_fs << _get_pid() << ":";
	_fs << "[" << _get_datetime() << "]:";
	_fs << "[" << level << "]:";
	_fs << msg << std::endl;
}

const std::string zn_buffered_log::endl = "LOG_END";
void zn_buffered_log::_log_msg(const std::string &level, const std::string &msg)
{
	if (msg.compare(endl) == 0)
	{
		zn_log::_log_msg(level, _buffer);
		_buffer = "";
	}
	else
	{
		_buffer += msg;
	}
}

bool zn_logger::_load_log_msg(const char* message_file)
{
	SI_Error ret =_ini.LoadFile(message_file);
	if (ret != 0)
	{
		ZN_THROW(zn_logger_exception, "Open log file error.");
	}
	return true;
}

std::string zn_logger::_get_log_msg(const char* log_id)
{
	const char *value = _ini.GetValue(INI_SECTION, log_id, NULL);
	if(value == NULL)
	{
		ZN_THROW(zn_logger_exception, "Log Number does not exist. Log ID = ", log_id);
	}
	return std::string(value);
}

void zn_logger::log(const char *log_id, ...)
{
	std::string log_fmt = zn_logger::_get_log_msg(log_id);
	log_fmt.insert(0, " ");
	log_fmt.insert(0, log_id);

	int log_level = 0;
	switch(log_id[1])
	{
		case '1':
			log_level = LOG_INFO;
			break;
		case '2':
			log_level = LOG_WARNING;
			break;
		case '3':
			log_level = LOG_ERR;
			break;
		case '0':
		default:
			log_level = LOG_DEBUG;
			break;
	}

	va_list	ap;
	va_start(ap, log_id);
	vsyslog(log_level, log_fmt.c_str(), ap);
	va_end(ap);
}

zn_logger& zn_logger::instance()
{
	if(!_instance._is_opened)
	{
		const char *log_facility_name[] = {"LOCAL0","LOCAL1","LOCAL2","LOCAL3",
											"LOCAL4","LOCAL5","LOCAL6","LOCAL7"};
		std::map<std::string,int> facility_map;
		for(int i = 0; i < 8; i++ )
		{
			facility_map[log_facility_name[i]] = (i+16) << 3;
		}

		std::string log_facility = zn_config_manager::instance().get("LOG_FACILITY");
		std::transform(log_facility.begin(), log_facility.end(), log_facility.begin(), toupper);
		if(facility_map.find(log_facility) == facility_map.end())
		{
			ZN_THROW(zn_logger_exception, "Parameter LOG_FACILITY was not correct.");
		}

		std::string file_name = zn_config_manager::instance().get("LOG_TEMPLATE_FILEPATH");
		if(!_instance._load_log_msg(file_name.c_str()))
		{
			ZN_THROW(zn_logger_exception, "Load msg file error. file = ", file_name);
		}

		openlog(_instance._prg_name.c_str(), LOG_NDELAY | LOG_PID, facility_map[log_facility]);
		_instance._is_opened = true;
	}
	return _instance;
}
