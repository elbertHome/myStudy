#pragma once

#include <unistd.h>

#include "zn_exception.hpp"

enum zn_retry_result
{
	ZN_RETRY_SUCCESS = 0,
	ZN_RETRY_FAIL,
	ZN_RETRY_TIMEOUT
};

template <typename CLASS_T, typename MEMBER_F, typename RETURN_T>
class zn_retry_adapter
{
	protected:
		CLASS_T *_class;
		MEMBER_F _func;

	public:
		zn_retry_adapter(CLASS_T *c, MEMBER_F f):
			_class(c)
		{
			_func = f;
		}

		virtual RETURN_T process() = 0;
};

template <typename CLASS_T, typename MEMBER_F, typename RETURN_T, typename T1, typename T2>
class zn_static_retry_adapter2 : public zn_retry_adapter<CLASS_T, MEMBER_F, RETURN_T>
{
	private:
		T1 _v1;
		T2 _v2;

	public:
		zn_static_retry_adapter2(CLASS_T *c, MEMBER_F f, T1 v1, T2 v2):
			zn_retry_adapter<CLASS_T, MEMBER_F, RETURN_T>(c, f),
			_v1(v1),
			_v2(v2)
		{}

		RETURN_T process()
		{
			MEMBER_F f = this->_func;
			return f(_v1, _v2);
		}
};

template <typename CLASS_T, typename MEMBER_F, typename RETURN_T, typename T1>
class zn_retry_adapter1 : public zn_retry_adapter<CLASS_T, MEMBER_F, RETURN_T>
{
	private:
		T1 _v1;

	public:
		zn_retry_adapter1(CLASS_T *c, MEMBER_F f, T1 v1):
			zn_retry_adapter<CLASS_T, MEMBER_F, RETURN_T>(c, f),
			_v1(v1)
		{}

		RETURN_T process()
		{
			MEMBER_F f = this->_func;
			return (this->_class->*f)(_v1);
		}
};

template <typename CLASS_T, typename MEMBER_F, typename RETURN_T, typename T1, typename T2>
class zn_retry_adapter2 : public zn_retry_adapter<CLASS_T, MEMBER_F, RETURN_T>
{
	private:
		T1 _v1;
		T2 _v2;

	public:
		zn_retry_adapter2(CLASS_T *c, MEMBER_F f, T1 v1, T2 v2):
			zn_retry_adapter<CLASS_T, MEMBER_F, RETURN_T>(c, f),
			_v1(v1),
			_v2(v2)
		{}

		RETURN_T process()
		{
			MEMBER_F f = this->_func;
			return (this->_class->*f)(_v1, _v2);
		}
};

template <typename CLASS_T, typename MEMBER_F, typename RETURN_T, typename T1, typename T2, typename T3, typename T4>
class zn_retry_adapter4 : public zn_retry_adapter<CLASS_T, MEMBER_F, RETURN_T>
{
	private:
		T1 _v1;
		T2 _v2;
		T3 _v3;
		T4 _v4;

	public:
		zn_retry_adapter4(CLASS_T *c, MEMBER_F f, T1 v1, T2 v2, T3 v3, T4 v4):
			zn_retry_adapter<CLASS_T, MEMBER_F, RETURN_T>(c, f),
			_v1(v1),
			_v2(v2),
			_v3(v3),
			_v4(v4)
		{}

		RETURN_T process()
		{
			MEMBER_F f = this->_func;
			return (this->_class->*f)(_v1, _v2, _v3, _v4);
		}
};

template <typename CLASS_T, typename MEMBER_F, typename RETURN_T>
class zn_retriable
{
	private:
		zn_retry_adapter<CLASS_T, MEMBER_F, RETURN_T> *_adapter;

		int _retry_times;
		int _delay_seconds;
		int _timeout_seconds;

	public:
		zn_retriable(zn_retry_adapter<CLASS_T, MEMBER_F, RETURN_T> *adapter, int retry_times = 3,
				int delay_seconds = 3, int timeout_seconds = 0):
			_adapter(adapter),
			_retry_times(retry_times),
			_delay_seconds(delay_seconds),
			_timeout_seconds(timeout_seconds)
		{}

		~zn_retriable()
		{}

		void init()
		{
			if (_timeout_seconds != 0)
			{
				// TODO: add native time out stuff.
			}
		}

		RETURN_T process()
		{
			int i = 0;
			while (true)
			{
				try
				{
					return _adapter->process();
				}
				catch (const zn_retry_exception &e)
				{
					if (i >= _retry_times)
					{
						ZN_THROW(zn_retry_exceed_exception, "max retry times exceeded.");
					}
					sleep(_delay_seconds);
				}
				++i;
			}
		}
};
