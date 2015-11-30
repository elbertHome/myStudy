#pragma once

#include <string>

#include <cerrno>

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>

#include "zn_exception.hpp"

class zn_exclusive_lock
{
	private:
		std::string _sem_name;
		unsigned int _sem_value;
		time_t _timeout;

		sem_t *_sem;

	public:
		zn_exclusive_lock(const std::string &sema_name,
				unsigned int sem_value,
				time_t timeout = 900):
			_sem_name(sema_name),
			_sem_value(sem_value),
			_sem(NULL)
		{}

		~zn_exclusive_lock()
		{
			if (_sem)
			{
				sem_post(_sem);
				sem_close(_sem);
			}
		}

		bool lock()
		{
			static const int oflag = O_CREAT; //| O_EXCL;
			static const mode_t mode = S_IRUSR | S_IWUSR;
			_sem = sem_open(_sem_name.c_str(), oflag, mode, _sem_value);
			if (_sem == SEM_FAILED)
			{
				return false;
			}

			struct timespec ts = {0, 0};
			time(&ts.tv_sec);
			ts.tv_sec += _timeout;
			int ret = sem_timedwait(_sem, &ts);

			if (ret != 0)
			{
				if (errno == ETIMEDOUT)
				{
					ZN_THROW(zn_exclusive_lock_timeout_exception, "wait semaphore timeout.");
				}
				else
				{
					ZN_THROW(zn_exclusive_lock_exception, "semaphore error.");
				}
			}

			return true;
		}

		int unlink()
		{
			return sem_unlink(_sem_name.c_str());
		}
};
