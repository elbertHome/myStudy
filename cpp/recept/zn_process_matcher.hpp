#pragma once
#include <vector>

#include <sys/types.h>

#include "zn_exception.hpp"

class zn_fs_proc
{
	private:
		pid_t _pid;
		std::string _cmdline;

	public:
		zn_fs_proc(const std::string &pid_string);

		std::string get_cmdline_column(unsigned int column);

		pid_t pid() const
		{
			return _pid;
		}
};

typedef std::vector<zn_fs_proc> proc_list;
typedef std::vector<pid_t> pid_list;

class zn_proc_processor
{
	private:
		proc_list _list;

		proc_list _pidlist_to_proclist(const pid_list &list);
		pid_list _get_pidlist();

	public:
		zn_proc_processor();
		zn_proc_processor(const pid_list &list);

		typedef proc_list::iterator iterator;

		iterator begin()
		{
			return _list.begin();
		}

		iterator end()
		{
			return _list.end();
		}
};

class zn_process_matcher
{
	private:
		pid_list _result;

		pid_list _find_column(const std::string &name, unsigned int column);

	public:
		static const unsigned int COMMAND_COLUMN = 0;
		static const unsigned int MAILID_COLUMN = 3;

		inline zn_process_matcher &find_mailid(const std::string &mailid);
		inline zn_process_matcher &find_process(const std::string &process_name);

		inline pid_list get_result();
};
