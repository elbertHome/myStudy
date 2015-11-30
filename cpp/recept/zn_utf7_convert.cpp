#include <memory>
#include <cstring>
#include <iconv.h>
#include <cerrno>
#include <iostream>
#include <ctype.h>
#include "zn_utf7_convert.hpp"
#include "zn_exception.hpp"
#include "zn_misc.hpp"

static const char* UTF8_STRING = "UTF-8";
static const char* UTF7_STRING = "UTF-7";
static const size_t BUFF_SIZE = 256;

static void check_folder_name_length(std::string& folder_path)
{
	const size_t FOLDER_NAME_MAX_LEN = 64;
	const size_t FOLDER_PATH_MAX_LEN = 248;

	if(folder_path.size() > FOLDER_PATH_MAX_LEN)
	{
		ZN_THROW(zn_folder_name_length_exception,
				folder_path.c_str());
	}

	std::string folder_name;
	for(size_t i = 0; i <= folder_path.size(); i++)
	{
		if(i == folder_path.size() ||
				folder_path[i] == '/')
		{
			if(folder_name.size() > FOLDER_NAME_MAX_LEN)
			{
				ZN_THROW(zn_folder_name_length_exception,
						folder_path.c_str(), folder_name.c_str());
			}
			folder_name.clear();
			continue;
		}
		folder_name.push_back(folder_path[i]);
	}
}

std::string zn_utf7_converter::_convert_utf8_to_utf7(std::string in_str)
{
//	in_str.push_back('a');
	size_t in_buf_len = in_str.size();
	char in_buf[in_buf_len + 1];
	char *p_in_buf = in_buf;
	size_t out_buf_len = BUFF_SIZE;
	char out_buf[BUFF_SIZE + 1];
	char *p_out_buf = out_buf;
	char err_str[BUFF_SIZE + 1];
	std::string out_str;
	iconv_t _iconv = iconv_open(UTF7_STRING, UTF8_STRING);

	if(_iconv == (iconv_t)-1)
	{
		ZN_THROW(zn_folder_name_convert_exception,
				errno,
				strerror_r(errno, err_str, BUFF_SIZE));
	}

	memset(in_buf, 0, sizeof(in_buf));
	memset(err_str, 0, BUFF_SIZE);

	std::strcpy(in_buf, in_str.c_str());

	while(in_buf_len != (size_t)-1)
	{
		memset(out_buf, 0, BUFF_SIZE + 1);
		p_out_buf = out_buf;
		out_buf_len = BUFF_SIZE;
		if(in_buf_len == 0)
		{
			// 変換元の末尾がascii文字ではない場合、
			// 変換が完了した後も、 まだ変換できていないbitが残っている可能性がある。
			// 上記を対応するため、iconvのステータスをresetさせる。
			p_in_buf = NULL;

			//ループを抜けるための減算
			in_buf_len--;
		}
		if(iconv(_iconv, &p_in_buf, &in_buf_len, &p_out_buf, &out_buf_len) == (size_t)-1)
		{
			if(errno != E2BIG)
			{
				iconv_close(_iconv);
				ZN_THROW(zn_folder_name_convert_exception,
						errno,
						strerror_r(errno, err_str, BUFF_SIZE));
			}
		}
		out_str.append(out_buf);
	}

	iconv_close(_iconv);

	return out_str;
}

std::string zn_utf7_converter::_convert_utf8_to_modified_utf7(const std::string &in_str)
{
	std::string out_str;
	std::string tmp_str;

	for(size_t i = 0; i < in_str.size(); i++)
	{
		if(!isprint(in_str[i]))
		{
			tmp_str.push_back(in_str[i]);
			if(i == in_str.size() - 1 ||
				isprint(in_str[i + 1]))
			{
				tmp_str = _convert_utf8_to_utf7(tmp_str);
				tmp_str.replace(0,1, "&");
				replace_charactor_all(tmp_str, (char*)"/", (char*)",");
				out_str.append(tmp_str);
				tmp_str.clear();
			}
		}
		else
		{
			out_str.push_back(in_str[i]);
			if(in_str[i] == '&')
			{
				out_str.push_back('-');
			}
		}
	}

	replace_charactor_all(out_str, (char*)"\\", (char*)"\\\\");
	replace_charactor_all(out_str, (char*)"\"", (char*)"\\\"");

	return out_str;
}


std::string zn_utf7_converter::zn_convert_folder_path(const std::string &in_str)
{
	std::string out_str = _convert_utf8_to_modified_utf7(in_str);
	check_folder_name_length(out_str);
	return out_str;
}

//int main()
//{
//	std::string str;
//	std::cout << "after = " << str << std::endl;
//	return 0;
//}
