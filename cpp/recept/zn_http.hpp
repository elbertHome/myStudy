#pragma once

#include <string>
#include <fstream>
#include "curl/curl.h"

#include "zn_exception.hpp"
#include "zn_folder_info.hpp"

class zn_http
{
	private:
		CURL *_curl;

		bool _perform(const std::string &url, const std::string &cookie);

	public:
		zn_http();
		~zn_http();

		static inline void init()
		{
			if (curl_global_init(CURL_GLOBAL_DEFAULT) != 0)
			{
				ZN_THROW(zn_http_common_exception, "global init error.");
			}
		}
		static inline void cleanup()
		{
			curl_global_cleanup();
		}

		bool post(const std::string &url, const std::string &postdata, const std::string &cookie = std::string());
		bool get(const std::string &url, const std::string &cookie = std::string());

		virtual size_t on_write(char *ptr, size_t size, size_t nmemb) = 0;
};

class zn_file_downloader : public zn_http
{
	private:
		std::ofstream _ofs;
		std::string _output_directory;

	public:
		zn_file_downloader():
			_output_directory("./")
		{}
		zn_file_downloader(const std::string &output_directory):
			_output_directory(output_directory)
		{}

		virtual size_t on_write(char *ptr, size_t size, size_t nmemb);
		bool download(const std::string &url, const std::string &cookie = std::string(), const std::string &output_filename = std::string());
};
