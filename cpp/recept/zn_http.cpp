#include "zn_http.hpp"
#include "zn_xml.hpp"

#include "zn_exception.hpp"

// wrap function for curl
static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	zn_http *znp= (zn_http *)userdata;
	return znp->on_write(ptr, size, nmemb);
}

// zn_http
zn_http::zn_http()
{
	_curl = curl_easy_init();
	if (_curl == NULL)
	{
		ZN_THROW(zn_http_common_exception, "easy init error.");
	}

	curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYHOST, 0L);
}

zn_http::~zn_http()
{
	if (_curl)
	{
		curl_easy_cleanup(_curl);
		_curl = NULL;
	}
}

bool zn_http::_perform(const std::string &url, const std::string &cookie)
{
	curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());

	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, this);
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, write_callback);

	if (cookie.length() != 0)
	{
		curl_easy_setopt(_curl, CURLOPT_COOKIE, cookie.c_str());
	}

	CURLcode retcode = curl_easy_perform(_curl);
	if (retcode != CURLE_OK)
	{
		ZN_THROW(zn_http_transfer_exception, retcode);
		return false;
	}

	return true;
}

bool zn_http::post(const std::string &url, const std::string &data, const std::string &cookie)
{
	curl_easy_setopt(_curl, CURLOPT_POST, 1L);
	curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, data.c_str());

	return _perform(url, cookie);
}

bool zn_http::get(const std::string &url, const std::string &cookie)
{
	curl_easy_setopt(_curl, CURLOPT_POST, 0L);

	return _perform(url, cookie);
}

// zn_file_downloader
size_t zn_file_downloader::on_write(char *ptr, size_t size, size_t nmemb)
{
	size_t size_all = size * nmemb;
	_ofs.write(ptr, size_all);
	return size_all;
}

bool zn_file_downloader::download(const std::string &url, const std::string &cookies, const std::string &output_filename)
{
	if (_ofs.is_open())
	{
		_ofs.close();
	}

	std::string bfname;
	if (output_filename.length() == 0)
	{
		size_t found = url.find_last_of('/');
		if (found == std::string::npos)
		{
			return false;
		}
		bfname = url.substr(found + 1);
	}
	else
	{
		bfname = output_filename;
	}
	bfname = _output_directory + output_filename;
	_ofs.open(bfname.c_str());
	get(url, cookies);
	_ofs.close();

	return true;
}
