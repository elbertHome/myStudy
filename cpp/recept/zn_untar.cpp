#include <fstream>
#include <sstream>

#include <cstring>
#include <cmath>

#include "zn_untar.hpp"
#include "zn_tar_internal.hpp"
#include "zn_misc.hpp"
#include "zn_exception.hpp"

static unsigned int ascii2uint(std::string ascii_number);
static unsigned int base8to10(unsigned int num);
static bool all_zero(const char *buf, size_t size);

// untar stream
bool zn_untar_stream::_set_tar_info(const tar_header *p_header)
{
	if (strncmp(p_header->magic, TMAGIC, 5) != 0)
	{
		return false;
	}

	// common for ordinary file and long name
	_tar_info.size = base8to10(ascii2uint(std::string(p_header->size, sizeof(p_header->size))));
	_tar_info.bytes_left = _tar_info.size;

	if (p_header->typeflag == REGTYPE)
	{
		// don't set name for long filename as we have already set it.
		if (_status == ZN_TAR_HEADER)
		{
			_tar_info.name = p_header->name;
		}
		_tar_info.mtime = ascii2uint(std::string(p_header->mtime, sizeof(p_header->mtime)));
		_tar_info.gname = p_header->gname;

		// set status to content
		_status = ZN_TAR_CONTENT;
	}
	else if (p_header->typeflag == LFTYPE)
	{
		_tar_info.name.clear();
		_status = ZN_TAR_LONGNAME;
	}
	else
	{
		return false;
	}

	return true;
}

void zn_untar_stream::feed(const std::string &buf)
{
	append(buf);

	for (std::string block = shift(ZN_TAR_BLOCK_SIZE); block.length() != 0; block = shift(ZN_TAR_BLOCK_SIZE))
	{
		if (eof())
		{
			return;
		}

		if (_status == ZN_TAR_HEADER || _status == ZN_TAR_LONGNAME_HEADER)
		{
			const tar_header *p_header = reinterpret_cast<const tar_header *>(block.c_str());

			if ((*p_header->magic == 0) && all_zero(block.c_str(), ZN_TAR_BLOCK_SIZE))
			{
				_tar_end_count++;
			}
			else if (!_set_tar_info(p_header))
			{
				continue;
			}
		}
		else if (_status == ZN_TAR_CONTENT)
		{
			if (_tar_info.bytes_left > ZN_TAR_BLOCK_SIZE)
			{
				on_write(_tar_info, block);
				_tar_info.bytes_left -= ZN_TAR_BLOCK_SIZE;
			}
			else
			{
				on_write(_tar_info, block.substr(0, _tar_info.bytes_left));
				_tar_info.clear();

				_status = ZN_TAR_HEADER;
			}
		}
		else if (_status == ZN_TAR_LONGNAME)
		{
			if (_tar_info.bytes_left > ZN_TAR_BLOCK_SIZE)
			{
				_tar_info.name.append(block);
				_tar_info.bytes_left -= ZN_TAR_BLOCK_SIZE;
			}
			else
			{
				_tar_info.name.append(block.c_str());
				_tar_info.bytes_left = 0;

				_status = ZN_TAR_LONGNAME_HEADER;
			}
		}
		else
		{
		}
	}
}

size_t zn_untar_stream::on_write(const zn_tar_info &tar_info, const std::string &content)
{
	mkpath(tar_info.name);

	if (!writefile(tar_info.name, content, std::ofstream::app))
	{
		ZN_THROW(zn_untar_exception, "write file error", tar_info.name);
	}

	return content.length();
}

// zn_zimbra_untar_stream
bool zn_zimbra_untar_stream::_set_tar_info(const tar_header *p_header)
{
	bool ret = zn_untar_stream::_set_tar_info(p_header);
	if (ret)
	{
		_tar_info.name = _tfp.process(_tar_info.name);
	}
	return ret;
}

// misc function
unsigned int ascii2uint(std::string ascii_number)
{
	size_t pos = 0;

	// replace '0' to ' ' for compatible
	for (std::string::iterator it = ascii_number.begin(); it != ascii_number.end() && *it == '0'; ++it)
	{
		*it = ' ';
	}

	// delete pre & trailing space
	while ((pos = ascii_number.find(' ')) != std::string::npos)
	{
		ascii_number.erase(pos, 1);
	}

	std::istringstream iss(ascii_number);
	unsigned int ret = 0;
	iss >> ret;

	return ret;
}

unsigned int base8to10(unsigned int num)
{
	unsigned int ret = 0;
	int level = 0;
	while (num)
	{
		ret += ((num % 10) * pow(8, level++));
		num /= 10;
	}

	return ret;
}

bool all_zero(const char *buf, size_t size)
{
	for (size_t i = 0; i != size; ++i)
	{
		if (buf[i])
		{
			return false;
		}
	}
	return true;
}
