#pragma once

#include <vector>
#include <string>
#include <memory>

// enable this option for use the deprecated but clear api.
#define LDAP_DEPRECATED 1
#include <ldap.h>

class zn_ldap_message
{
	private:
		LDAP *_ldap;
		LDAPMessage *_msg;

	public:
		zn_ldap_message(LDAP *ldap, LDAPMessage *msg):
			_ldap(ldap),
			_msg(msg)
		{}

		~zn_ldap_message()
		{
			if (_msg != NULL) ldap_msgfree(_msg);
		}

		std::string get_value(const std::string &attribute);
};

typedef std::auto_ptr<zn_ldap_message> zn_ldap_message_p;

class zn_ldap
{
	private:
		LDAP *_ldap;

		static const char *DN;
		static const char *PASSWORD;

	public:
		zn_ldap():
			_ldap(NULL)
		{}

		~zn_ldap()
		{
			if (_ldap != NULL) ldap_unbind(_ldap);
		}

		bool init(const std::string &ldap_url, const char *dn = DN, const char *password = PASSWORD);

		zn_ldap_message_p search(const std::string &base, const std::string &filter,
				int limit = 10, int timeout = 0);
};

class zn_zimbra_ldap
{
	private:
		static const char *BASE;

	public:
		typedef std::vector<std::string> attributes;
		typedef std::vector<std::string> values;

		static values get_zimbra_ldap_values(const std::string &login_id, const attributes &attr);
};
