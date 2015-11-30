#pragma once

#include "zn_http.hpp"

class zn_soap_requst : public zn_http
{
	private:
		std::string _content;
		std::string _soap_url;

	public:
		zn_soap_requst(const std::string &url):
			_soap_url(url)
		{}

		virtual size_t on_write(char *ptr, size_t size, size_t nmemb);
		std::string post(const std::string &postdata);
};

class zn_zimbra_soap
{
	private:
		zn_soap_requst _zimbra_soap;

		std::string _zimbraid;
		std::string _admin_token;
		std::string _delegate_token;

		std::string _delegate_auth();

		std::string _send_soap_request(const std::string &soap_content);

		bool _modify_account(const std::string &password, bool include_spam_flag);
		bool _set_password(const std::string &password);

		std::string _get_md_random_password_prefix();

	public:
		zn_zimbra_soap(const std::string &zimbra_host, const std::string &zimbraid);

		std::string change_password(const std::string &original_password, bool include_spam_flag);
		std::string get_info();
		std::string get_delegate_token();
};
