#include "zn_metafile.hpp"

static size_t find_nonliteral(const std::string &string, char c)
{
	if (c == '"')
	{
		return std::string::npos;
	}

	const char *p_str = string.c_str();
	bool quote_flag = false;
	for (size_t i = 0; i != string.length(); ++i)
	{
		if (p_str[i] == '"')
		{
			quote_flag = !quote_flag;
		}
		else if ((p_str[i] == c) && !quote_flag)
		{
			return i;
		}
	}

	return std::string::npos;
}

bool zn_metafile::load_meta(const std::string &content)
{
	if (content.length() == 0)
	{
		return false;
	}

	// copy to buf without "{}"
	std::string buf(content.begin() + 1, content.end() - 2);
	// append "," for search
	buf.append(",");

	size_t pos;
	while ((pos = find_nonliteral(buf, ',')) != std::string::npos)
	{
		// advanced to next
		std::string kv_pair = buf.substr(0, pos);
		buf.erase(0, pos+1);

		if ((pos = kv_pair.find(':')) != std::string::npos)
		{
			std::string key = kv_pair.substr(0, pos);
			std::string value = kv_pair.substr(pos + 1, kv_pair.length() - pos);

			while ((pos = key.find('"')) != std::string::npos)
			{
				key.erase(pos, 1);
			}
			while ((pos = value.find('"')) != std::string::npos)
			{
				value.erase(pos, 1);
			}

			_meta.insert(make_pair(key,value));

		}
	}

	is_read = get_value("unread").compare("0") == 0?false:true;
	date = get_value("date");
	id = from_string<int>(get_value("id"));
	folder_id = from_string<int>(get_value("folder_id"));

	return true;
}

std::string zn_metafile::get_value(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator it_find = _meta.find(key);
	if(it_find == _meta.end())
	{
		return std::string();
	}
	else
	{
		return it_find->second;
	}
}

bool zn_metafile::set_value(const std::string &key, const std::string &value)
{
	_meta[key] = value;
	return true;
}

std::string zn_metafile::dump()
{
	std::string ret;

	for (std::map<std::string, std::string>::iterator it = _meta.begin(); it != _meta.end(); ++it)
	{
		ret += it->first;
		ret += " => ";
		ret += it->second;
		ret += "\n";
	}

	return ret;
}

std::string zn_metafile::serialize()
{
	std::string ret;

	ret += to_string(id);
	ret += ",";
	ret += to_string(folder_id);
	ret += ",";
	ret += is_read?"read":"unread";
	ret += ",";
	ret += date;

	return ret;
}

bool zn_metafile::deserialize(const std::string &data)
{
	std::vector<std::string> v = split(data, ',');
	id = from_string<int>(v[0]);
	folder_id = from_string<int>(v[1]);
	is_read = v[2].compare("read")==0?true:false;
	date = v[3];

	return true;
}
