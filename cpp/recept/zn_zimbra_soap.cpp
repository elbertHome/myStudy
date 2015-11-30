#include "zn_zimbra_soap.hpp"

#include "zn_xml.hpp"
#include "zn_message_digest.hpp"
#include "zn_network_req_generator.hpp"

#include "zn_misc.hpp"

#include "zn_exception.hpp"
#include "zn_retriable.hpp"
#include "zn_logger.hpp"
#include "zn_config_manager.hpp"

// zn_soap_requst
size_t zn_soap_requst::on_write(char *ptr, size_t size, size_t nmemb)
{
	size_t size_all = size * nmemb;
	_content.append(ptr, size_all);
	return size_all;
}

std::string zn_soap_requst::post(const std::string &postdata)
{
	_content.clear();
	zn_http::post(_soap_url, postdata);
	// TODO:
	// try catch and check _content for error.
	return _content;
}

typedef std::string (zn_soap_requst::*zn_soap_requst_post_f)(const std::string &postdata);
typedef zn_retry_adapter1<zn_soap_requst, zn_soap_requst_post_f, std::string, const std::string &> zn_soap_requst_retry_adapter;

std::string zn_zimbra_soap::_send_soap_request(const std::string &soap_content)
{
	zn_soap_requst_retry_adapter retry_adapter(&_zimbra_soap, &zn_soap_requst::post, soap_content);
	zn_retriable<zn_soap_requst, zn_soap_requst_post_f, std::string> auto_retry_soap_request(&retry_adapter);
	return auto_retry_soap_request.process();
}

// zn_zimbra_soap
std::string zn_zimbra_soap::_delegate_auth()
{
	std::string request = zn_network_req_generator::get_delegate_auth_request(
			_admin_token,
			_zimbraid);
	std::string ret = _send_soap_request(request);

	/*
	std::string ret = _zimbra_soap.post(zn_network_req_generator::get_delegate_auth_request(
				_admin_token,
				_zimbraid));
	*/

	zn_logger::instance().debug() << ret << zn_buffered_log::endl;

	return ret;
}

bool zn_zimbra_soap::_modify_account(const std::string &password, bool include_spam_flag)
{
	std::string request = zn_network_req_generator::get_modify_account_request(
				_admin_token,
				_zimbraid,
				zn_soap_digest::md_ssha(password),
				password,
				include_spam_flag?"TRUE":"FALSE");
	std::string ret = _send_soap_request(request);
	/*
	std::string ret = _zimbra_soap.post(zn_network_req_generator::get_modify_account_request(
				_admin_token,
				_zimbraid,
				zn_soap_digest::md_ssha(password),
				password,
				get_current_datetime() + "Z"));
	*/

	zn_logger::instance().debug() << ret << zn_buffered_log::endl;

	return true;
}

bool zn_zimbra_soap::_set_password(const std::string &password)
{
	std::string request = zn_network_req_generator::get_set_password_request(
				_admin_token,
				_zimbraid,
				password);
	std::string ret = _send_soap_request(request);
	/*
	std::string ret = _zimbra_soap.post(zn_network_req_generator::get_set_password_request(
				_admin_token,
				_zimbraid,
				password));
	*/

	zn_logger::instance().debug() << ret << zn_buffered_log::endl;

	return true;
}

// ・文字種 ： 英大文字/小文字・数字
// ・大文字のO(オー)/小文字のl(エル)/大文字のI(アイ)は使用しない
std::string zn_zimbra_soap::_get_md_random_password_prefix()
{
	static const size_t RANDOM_PASSWORD_SIZE = 4;
	static const char PASSWORD_TABLE[] = {
'0','1','2','3','4','5','6','7','8','9',
'a','b','c','d','e','f','g','h','i','j','k','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
'A','B','C','D','E','F','G','H','J','K','L','M','N','P','Q','R','S','T','U','V','W','X','Y','Z'
	};

	char buf[RANDOM_PASSWORD_SIZE];
	//srand(time(NULL));
	srand(0);
	for (size_t i = 0; i != RANDOM_PASSWORD_SIZE; ++i)
	{
		buf[i] = PASSWORD_TABLE[rand() % sizeof(PASSWORD_TABLE)];
	}

	return std::string(buf, RANDOM_PASSWORD_SIZE);
}

zn_zimbra_soap::zn_zimbra_soap(const std::string &zimbra_host, const std::string &zimbraid):
	_zimbra_soap(zn_network_req_generator::get_url(
				zn_config_manager::instance().get("MBS_SOAP_URL_PREFIX"),
				zimbra_host,
				zn_config_manager::instance().get("MBS_SOAP_PORT_NO"),
				zn_config_manager::instance().get("MBS_SOAP_URL_SUFFIX"))),
	_zimbraid(zimbraid),
	_admin_token(zn_config_manager::instance().get("MBS_SOAP_ADMINAUTHTOKEN"))
{
}

std::string zn_zimbra_soap::change_password(const std::string &original_password, bool include_spam_flag)
{
	std::string new_password = _get_md_random_password_prefix();
	new_password += original_password;

	_modify_account(new_password, include_spam_flag) && _set_password(new_password);

	return new_password;
}

std::string zn_zimbra_soap::get_info()
{
	std::string auth_token = get_delegate_token();

	std::string request = zn_network_req_generator::get_info_request(auth_token);
	std::string ret = _send_soap_request(request);
	/*
	std::string ret = _zimbra_soap.post(zn_network_req_generator::get_info_request(auth_token));
	*/

	zn_logger::instance().debug() << ret << zn_buffered_log::endl;

	return ret;
}

std::string zn_zimbra_soap::get_delegate_token()
{
	if (_delegate_token.length() == 0)
	{
		_delegate_token = zn_xml::get_element(_delegate_auth(), "authToken");
	}

	return _delegate_token;
}
