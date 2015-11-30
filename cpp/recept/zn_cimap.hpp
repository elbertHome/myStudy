#pragma once

#include <map>
#include <memory>
#include <string>

#include <unistd.h>

enum zn_cimap_option
{
	CIMAP_USERNAME,
	CIMAP_PASSWORD,
	CIMAP_MAILBOXNAME,
	CIMAP_FLAGS,
	CIMAP_DATETIME,
	CIMAP_LITERAL,
};

class zn_cimap_interaction
{
	private:
		std::map<zn_cimap_option, std::string> _options;
		int _sockfd;

		bool _response_status(const std::string &response, const std::string &status);
		std::string _get_response_content(const std::string &response);
		std::string _get_last_response(const std::string &response);

		bool _check_response(const std::string &response);

	public:
		zn_cimap_interaction():
			_sockfd(0)
		{}

		zn_cimap_interaction &set_option(zn_cimap_option opt, const std::string &value);
		zn_cimap_interaction &clear_options();

		virtual bool perform() = 0;
		//bool errno();

	friend class zn_cimap_cmd_welcome;
	friend class zn_cimap_cmd_login;
	friend class zn_cimap_cmd_create;
	friend class zn_cimap_cmd_append;
	friend class zn_cimap_cmd_logout;
};

class zn_cimap_cmd_welcome : public zn_cimap_interaction
{
	private:
	public:
		zn_cimap_cmd_welcome(const int &fd)
		{
			_sockfd = fd;
		}

		virtual bool perform();
};

class zn_cimap_cmd_login : public zn_cimap_interaction
{
	private:
	public:
		zn_cimap_cmd_login(const int &fd)
		{
			_sockfd = fd;
		}

		virtual bool perform();
};

class zn_cimap_cmd_create : public zn_cimap_interaction
{
	private:
	public:
		zn_cimap_cmd_create(const int &fd)
		{
			_sockfd = fd;
		}

		virtual bool perform();
};

class zn_cimap_cmd_append : public zn_cimap_interaction
{
	private:
	public:
		zn_cimap_cmd_append(const int &fd)
		{
			_sockfd = fd;
		}

		virtual bool perform();
};

class zn_cimap_cmd_logout : public zn_cimap_interaction
{
	private:
	public:
		zn_cimap_cmd_logout(const int &fd)
		{
			_sockfd = fd;
		}

		virtual bool perform();
};

typedef std::auto_ptr<zn_cimap_interaction> zn_cimap_interaction_p;

class zn_cimap_session
{
	private:
		int _sockfd;

	public:
		zn_cimap_session():
			_sockfd(0)
		{}
		~zn_cimap_session()
		{
			if (_sockfd != 0)
			{
				close(_sockfd);
			}
		}

		bool init(const std::string &host, const std::string &port);

		template <typename T>
		zn_cimap_interaction_p get_cmd()
		{
			zn_cimap_interaction_p ret(dynamic_cast<zn_cimap_interaction *>(new T(_sockfd)));
			return ret;
		}
};

class zn_nsim_session
{
	private:
		zn_cimap_session _imap_session;

	public:
		bool init(const std::string &host, const std::string &port)
		{
			return _imap_session.init(host, port);
		}

		bool nsim_login(const std::string &username, const std::string &password);
};
