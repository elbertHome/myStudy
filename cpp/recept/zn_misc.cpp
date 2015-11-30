#include <string>
#include <fstream>

#include <cstring>
#include <cerrno>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

#include "zn_misc.hpp"

// copy from internet.
bool mkpath(const std::string &file_path)
{
	if (file_path.length() == 0)
	{
		return false;
	}

	std::vector<char> buf(file_path.length() + 1);
	char *local_path = &buf[0];
	memcpy(local_path, file_path.c_str(), file_path.length());

	for (char *p = strchr(local_path + 1, '/'); p; p = strchr(p + 1, '/'))
	{
		*p = '\0';
		if (mkdir(local_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
		{
			if (errno != EEXIST)
			{
				*p = '/';
				return false;
			}
		}
		*p = '/';
	}

	return true;
}

std::string readfile(const std::string &filename)
{
	static const size_t MAILFILE_BUF_SIZE = 4096;

	std::string ret;

	std::vector<char> buf(MAILFILE_BUF_SIZE);
	char *pbuf = &(buf.front());
	std::ifstream ifs(filename.c_str(), std::ifstream::in|std::ofstream::binary);
	while (ifs.good())
	{
		ifs.read(pbuf, MAILFILE_BUF_SIZE);
		ret.append(pbuf, ifs.gcount());
	}
	return ret;
}

bool writefile(const std::string &filename, const std::string &data, std::ios_base::openmode mode)
{
	std::ofstream ofs(filename.c_str(), std::ofstream::binary | mode);
	ofs.write(data.c_str(), data.length());
	if (ofs.bad())
	{
		return false;
	}

	ofs.close();
	return true;
}

std::string dirname(const std::string &filename)
{
	size_t ret = filename.find_last_of("/");
	if (ret == std::string::npos)
	{
		return std::string(".");
	}
	else
	{
		return filename.substr(0, ret);
	}
}

std::string basename(const std::string &filename)
{
	size_t ret = filename.find_last_of("/");
	if (ret == std::string::npos)
	{
		return filename;
	}
	else
	{
		return filename.substr(ret + 1);
	}
}

bool rmdir_recursive(const std::string &dir, const std::string &stop_dir = "/var/tmp")
{
	if (dir.compare(stop_dir) == 0)
	{
		return true;
	}

	DIR *dirh = opendir(dir.c_str());
	struct dirent *dir_entry;
	int dir_count = 0;
	do
	{
		dir_entry = readdir(dirh);
		dir_count++;
	} while (dir_entry != NULL);
	closedir(dirh);

	if (dir_count == 3)
	{
		rmdir(dir.c_str());
		return rmdir_recursive(dirname(dir));
	}
	else
	{
		return true;
	}
}

bool remove_with_directory(const std::string &filename)
{
	if (remove(filename.c_str()) != 0)
	{
		return false;
	}

	rmdir_recursive(dirname(filename));
	return true;
}

std::string get_current_datetime()
{
	// YYYYMMDDhhmmssZ
	const static int DATE_BUFFER_SIZE = 15 + 1;

    time_t rawtime;
    struct tm *timeinfo;
    char buf[DATE_BUFFER_SIZE];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buf, DATE_BUFFER_SIZE, "%Y%m%d%H%M%S", timeinfo);

    return std::string(buf);
}

void get_word(const std::string &line, std::size_t &pos, std::string &word)
{
	std::size_t start_pos = pos;
	bool is_space = false;

	for(; pos < line.size(); pos++)
	{
		if(line[pos] == ' ')
		{
			is_space = true;
			break;
		}
	}

	word.assign(line, start_pos, pos - start_pos);
	if (is_space)
		pos++;

	return;
}

void replace_charactor_all(std::string& str, const char* search_char, const char* rep_char)
{
	size_t pos = 0;
	size_t rep_len = strlen(rep_char);
	size_t search_len = strlen(search_char);
	while((pos = str.find(search_char, pos)) != std::string::npos)
	{
		str.replace(pos, search_len, rep_char, rep_len);
		pos += rep_len;
	}
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

std::string join(std::vector<std::string> &v, char delim)
{
	std::string ret;
	std::vector<std::string>::iterator it;
	for (it = v.begin(); it != v.end() - 1; ++it)
	{
		ret += *it;
		ret += delim;
	}
	ret += *it;

	return ret;
}
