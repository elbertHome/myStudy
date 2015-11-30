#pragma once

#include <string>

class zn_stream
{
	private:
	public:
		virtual void feed(const std::string &buf) = 0;
		virtual std::string consume() = 0;

		zn_stream &operator<<(const std::string &buf)
		{
			feed(buf);
			return *this;
		}

		zn_stream &operator<<(zn_stream &stream)
		{
			feed(stream.consume());
			return *this;
		}
};

class zn_buffered_stream : public zn_stream
{
	private:
		std::string _buf;

	public:
		inline void append(const std::string &buf)
		{
			_buf += buf;
		}

		inline std::string shift()
		{
			return shift(_buf.length());
		}

		inline std::string shift(size_t n)
		{
			if (n > _buf.length())
			{
				return std::string();
			}

			std::string ret = _buf.substr(0, n);
			_buf.erase(0, n);
			return ret;
		}

		inline void unshift(const std::string &buf)
		{
			_buf.insert(0, buf);
		}

		inline void clear()
		{
			_buf.clear();
		}

		virtual void feed(const std::string &buf);
		virtual std::string consume();
};
