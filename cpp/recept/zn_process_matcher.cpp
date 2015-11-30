#include <string>
#include <algorithm>

#include <sys/types.h>
#include <dirent.h>

#include "zn_misc.hpp"
#include "zn_process_matcher.hpp"

inline bool is_digit(char c);

// zn_proc_cmdline
zn_fs_proc::zn_fs_proc(const std::string &pid_string)
{
	_pid = from_string<int>(pid_string);
	_cmdline = readfile("/proc/" + pid_string + "/cmdline");
}

std::string zn_fs_proc::get_cmdline_column(unsigned int column)
{
	for (unsigned int i = 0; i < _cmdline.length(); ++i)
	{
		if (column == 0)
		{
			return std::string(_cmdline.c_str() + i);
		}

		if (_cmdline[i] == '\0')
		{
			column--;
		}
	}

	return std::string();
}

// zn_proc_processor
pid_list zn_proc_processor::_get_pidlist()
{
	pid_list ret;

	DIR *dirp = opendir("/proc");
	if (dirp != NULL)
	{
		struct dirent *direntp = NULL;
		while ((direntp = readdir(dirp)) != NULL)
		{
			if ((direntp->d_type == DT_DIR) && is_digit(direntp->d_name[0]))
			{
				ret.push_back(from_string<pid_t>(direntp->d_name));
			}
		}
		closedir(dirp);

		std::sort(ret.begin(), ret.end());
	}

	return ret;
}

proc_list zn_proc_processor::_pidlist_to_proclist(const pid_list &list)
{
	proc_list ret;

	for (pid_list::const_iterator it = list.begin(); it != list.end(); ++it)
	{
		ret.push_back(zn_fs_proc(to_string(*it)));
	}

	return ret;
}

zn_proc_processor::zn_proc_processor()
{
	_list = _pidlist_to_proclist(_get_pidlist());
}

zn_proc_processor::zn_proc_processor(const pid_list &list)
{
	if (list.empty())
	{
		_list = _pidlist_to_proclist(_get_pidlist());
	}
	else
	{
		_list = _pidlist_to_proclist(list);
	}
}

// zn_process_watcher
pid_list zn_process_matcher::_find_column(const std::string &name, unsigned int column)
{
	zn_proc_processor pp(_result);

	pid_list ret;
	for (zn_proc_processor::iterator it = pp.begin(); it != pp.end(); ++it)
	{
		if (it->get_cmdline_column(column).find(name) != std::string::npos)
		{
			ret.push_back(it->pid());
		}
	}

	return ret;
}

zn_process_matcher &zn_process_matcher::find_process(const std::string &process_name)
{
	_result = _find_column(process_name, COMMAND_COLUMN);

	return *this;
}

zn_process_matcher &zn_process_matcher::find_mailid(const std::string &mailid)
{
	_result = _find_column(mailid, MAILID_COLUMN);

	return *this;
}

pid_list zn_process_matcher::get_result()
{
	return _result;
}

// misc
bool is_digit(char c)
{
	return (('0' <= c) && (c <= '9'));
}
