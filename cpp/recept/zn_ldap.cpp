#include <iostream>
#include <string>
#include <sstream>

#include <sys/time.h>

#include "zn_ldap.hpp"

#include "zn_exception.hpp"
#include "zn_retriable.hpp"
#include "zn_logger.hpp"
#include "zn_config_manager.hpp"

const char *zn_ldap::DN = "uid=zmreplica,cn=admins,cn=zimbra";
const char *zn_ldap::PASSWORD = "Decjapan";

// zn_ldap_message
std::string zn_ldap_message::get_value(const std::string &attribute)
{
	std::string ret;

	struct berval **bvalue_list = ldap_get_values_len(_ldap, _msg, attribute.c_str());
	if (bvalue_list == NULL)
	{
		//std::cout << "ldap_get_values error." << std::endl;
		ZN_THROW(zn_ldap_retry_exception, "ldap_get_values error.");
		return ret;
	}
	struct berval *bvalue = bvalue_list[0];
	ret = std::string(bvalue->bv_val, bvalue->bv_len);
	ldap_value_free_len(bvalue_list);

	return ret;
}

// zn_ldap
bool zn_ldap::init(const std::string &ldap_url, const char *dn, const char *password)
{
	int ret = 0;

	ret = ldap_initialize(&_ldap, ldap_url.c_str());
	if (ret != LDAP_SUCCESS)
	{
		//std::cout << ldap_err2string(ret) << std::endl;
		ZN_THROW(zn_ldap_retry_exception, ldap_err2string(ret));
		return false;
	}

	int version = LDAP_VERSION3;
	ret = ldap_set_option(_ldap, LDAP_OPT_PROTOCOL_VERSION, &version);
	if (ret != LDAP_OPT_SUCCESS)
	{
		// set option error.
		ZN_THROW(zn_ldap_retry_exception, ldap_err2string(ret));
		return false;
	}

	ret = ldap_simple_bind_s(_ldap, dn, password);
	if (ret != LDAP_SUCCESS)
	{
		//std::cout << ldap_err2string(ret) << std::endl;
		ZN_THROW(zn_ldap_retry_exception, ldap_err2string(ret));
		return false;
	}

	return true;
}

zn_ldap_message_p zn_ldap::search(const std::string &base, const std::string &filter, int limit, int timeout)
{
	LDAPMessage *msg = NULL;

	int ret = 0;
	ret = ldap_set_option(_ldap, LDAP_OPT_TIMELIMIT, &timeout);
	ret = ldap_search_ext_s(_ldap, base.c_str(), LDAP_SCOPE_SUBTREE, filter.c_str(), NULL, 0, NULL, NULL, NULL, limit, &msg);
	if (ret == LDAP_SUCCESS)
	{
		int ldap_count = ldap_count_entries(_ldap, msg);
		if (ldap_count == 1)
		{
			// normal route, nothing to do.
		}
		else if (ldap_count == 0)
		{
			ZN_THROW(zn_ldap_no_account_exception, "");
			return zn_ldap_message_p(NULL);
		}
		else
		{
			ZN_THROW(zn_ldap_retry_exception, "multi account found.");
			return zn_ldap_message_p(NULL);
		}
	}
	else
	{
		ZN_THROW(zn_ldap_retry_exception, ldap_err2string(ret));
		return zn_ldap_message_p(NULL);
	}

	return zn_ldap_message_p(new zn_ldap_message(_ldap, msg));
}

// zn_zimbra_ldap
typedef zn_ldap_message_p (zn_ldap::*zn_zimbra_ldap_f)(const std::string &, const std::string &, int, int);
typedef zn_retry_adapter4<zn_ldap, zn_zimbra_ldap_f, zn_ldap_message_p, const std::string &, const std::string &, int, int> zn_zimbra_ldap_retry_adapter;

//const char *zn_zimbra_ldap::BASE = "dc=ne,dc=jp";
const char *zn_zimbra_ldap::BASE = "";

zn_zimbra_ldap::values zn_zimbra_ldap::get_zimbra_ldap_values(const std::string &login_id, const zn_zimbra_ldap::attributes &attr)
{
	values ret;

	zn_ldap ldap;
	std::string oss = "ldap://" + zn_config_manager::instance().get("ZLDAP_HOST_NAME") +
		":" + zn_config_manager::instance().get("ZLDAP_PORT_NO");

	if (ldap.init(oss))
	{
		std::string oss_key = "(ezwebSubscriberId=" + login_id + "_*)";

		zn_zimbra_ldap_retry_adapter retry_adapter(&ldap, &zn_ldap::search, BASE, oss_key, 1 * 60, 0);
		zn_retriable<zn_ldap, zn_zimbra_ldap_f, zn_ldap_message_p> auto_retry_ldap_search(&retry_adapter);
		zn_ldap_message_p ldap_msg_p = auto_retry_ldap_search.process();
		//zn_ldap_message_p ldap_msg_p = ldap.search(BASE, oss_key.str());

		for (attributes::const_iterator it = attr.begin(); it != attr.end(); ++it)
		{
			ret.push_back(ldap_msg_p->get_value(*it));
		}
	}

	return ret;
}
