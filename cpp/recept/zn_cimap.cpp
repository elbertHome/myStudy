#include <string>
#include <vector>
#include <iostream>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "zn_cimap.hpp"
#include "zn_misc.hpp"

#include "zn_exception.hpp"
#include "zn_logger.hpp"

static const char *CIMAP_TAG = "00";
static const char *CIMAP_CMD_END = "\r\n";
static const int MAX_BUFFER = 1024;

// zn_cimap_interaction
bool zn_cimap_interaction::_response_status(const std::string &response, const std::string &status)
{
	std::string response_status = std::string(CIMAP_TAG);
	response_status += " ";
	response_status += status;

	return (response.compare(0, response_status.size(), response_status) == 0);
}

std::string zn_cimap_interaction::_get_response_content(const std::string &response)
{
	size_t space_pos = response.find(" ", strlen(CIMAP_TAG) + 1);
	if (space_pos == std::string::npos)
	{
		ZN_THROW(zn_cimap_response_error_exception, response);
	}

	return response.substr(space_pos);
}

std::string zn_cimap_interaction::_get_last_response(const std::string &response)
{
	static const int size_of_newline = 2;
	size_t newline_pos = response.rfind("\n", size_of_newline);
	if (newline_pos == std::string::npos)
	{
		return response;
	}
	else
	{
		return response.substr(newline_pos + 1);
	}
}

bool zn_cimap_interaction::_check_response(const std::string &response)
{
	/*
	std::string last_response = _get_last_response(response);

	if (_response_status(last_response, "NO"))
	{
		ZN_THROW(zn_cimap_no_exception, _get_response_content(response));
		return false;
	}
	else if (_response_status(last_response, "BAD"))
	{
		ZN_THROW(zn_cimap_bad_exception, _get_response_content(response));
		return false;
	}
	else if (_response_status(last_response, "OK") || _response_status(last_response, "BYE"))
	{
		// everything is ok.
	}
	else
	{
		ZN_THROW(zn_cimap_response_error_exception, last_response);
		return false;
	}
	*/

	return true;
}

zn_cimap_interaction &zn_cimap_interaction::set_option(zn_cimap_option opt, const std::string &value)
{
	_options[opt] = value;
	return *this;
}

zn_cimap_interaction &zn_cimap_interaction::clear_options()
{
	_options.clear();
	return *this;
}

// zn_cimap_cmd_welcome
bool zn_cimap_cmd_welcome::perform()
{
	std::vector<char> read_buf(MAX_BUFFER);
	ssize_t rret = read(_sockfd, &read_buf[0], MAX_BUFFER);
	if (rret == -1)
	{
		ZN_THROW(zn_cimap_read_exception, "");
		return false;
	}

	zn_logger::instance().debug() << "S:" << &read_buf[0] << zn_buffered_log::endl;

	return true;
}

// zn_cimap_cmd_login
bool zn_cimap_cmd_login::perform()
{
	std::string write_buf = CIMAP_TAG;
	write_buf += " ";
	write_buf += "LOGIN";
	write_buf += " ";
	write_buf += _options[CIMAP_USERNAME];
	write_buf += " ";
	write_buf += _options[CIMAP_PASSWORD];
	write_buf += CIMAP_CMD_END;

	size_t ret = write(_sockfd, write_buf.c_str(), write_buf.length());
	zn_logger::instance().debug() << "C: " << write_buf << zn_buffered_log::endl;

	if (ret != write_buf.length())
	{
		ZN_THROW(zn_cimap_write_exception, "");
		return false;
	}

	std::vector<char> read_buf(MAX_BUFFER);
	ssize_t rret = read(_sockfd, &read_buf[0], MAX_BUFFER);
	if (rret == -1)
	{
		ZN_THROW(zn_cimap_read_exception, "");
		return false;
	}

	zn_logger::instance().debug() << "S: " << &read_buf[0] << zn_buffered_log::endl;

	return _check_response(&read_buf[0]);
}

// zn_cimap_cmd_logout
bool zn_cimap_cmd_logout::perform()
{
	std::string write_buf = CIMAP_TAG;
	write_buf += " ";
	write_buf += "LOGOUT";
	write_buf += CIMAP_CMD_END;

	size_t ret = write(_sockfd, write_buf.c_str(), write_buf.length());
	zn_logger::instance().debug() << "C: " << write_buf << zn_buffered_log::endl;

	if (ret != write_buf.length())
	{
		ZN_THROW(zn_cimap_write_exception, "");
		return false;
	}

	std::vector<char> read_buf(MAX_BUFFER);
	char *bufp = &read_buf[0];
	do
	{
		ssize_t rret = read(_sockfd, bufp, MAX_BUFFER);
		if (rret == -1)
		{
			ZN_THROW(zn_cimap_read_exception, "");
			return false;
		}

		zn_logger::instance().debug() << "S: " << bufp << zn_buffered_log::endl;
	} while (strncmp(CIMAP_TAG, bufp, 3) == 0);

	return _check_response(&read_buf[0]);
}

// zn_cimap_cmd_create
bool zn_cimap_cmd_create::perform()
{
	std::string write_buf = CIMAP_TAG;
	write_buf += " ";
	write_buf += "CREATE";
	write_buf += " ";
	write_buf += _options[CIMAP_MAILBOXNAME];
	write_buf += CIMAP_CMD_END;

	size_t ret = write(_sockfd, write_buf.c_str(), write_buf.length());
	zn_logger::instance().debug() << "C: " << write_buf << zn_buffered_log::endl;

	if (ret != write_buf.length())
	{
		ZN_THROW(zn_cimap_write_exception, "");
		return false;
	}

	std::vector<char> read_buf(MAX_BUFFER);
	ssize_t rret = read(_sockfd, &read_buf[0], MAX_BUFFER);
	if (rret == -1)
	{
		ZN_THROW(zn_cimap_read_exception, "");
		return false;
	}

	zn_logger::instance().debug() << "S: " << &read_buf[0] << zn_buffered_log::endl;

	return _check_response(&read_buf[0]);
}

// zn_cimap_cmd_append
bool zn_cimap_cmd_append::perform()
{
	std::string write_buf = CIMAP_TAG;
	write_buf += " ";
	write_buf += "APPEND";
	write_buf += " ";
	write_buf += _options[CIMAP_MAILBOXNAME];
	write_buf += " ";
	if (_options[CIMAP_FLAGS].length() != 0)
	{
		write_buf += _options[CIMAP_FLAGS];
		write_buf += " ";
	}
	if (_options[CIMAP_DATETIME].length() != 0)
	{
		write_buf += _options[CIMAP_DATETIME];
		write_buf += " ";
	}
	write_buf += "{";
	write_buf += to_string(_options[CIMAP_LITERAL].length());
	write_buf += "}";
	write_buf += CIMAP_CMD_END;

	size_t ret = write(_sockfd, write_buf.c_str(), write_buf.length());
	zn_logger::instance().debug() << "C: " << write_buf << zn_buffered_log::endl;

	if (ret != write_buf.length())
	{
		ZN_THROW(zn_cimap_write_exception, "");
		return false;
	}

	std::vector<char> read_buf(MAX_BUFFER);
	ssize_t rret = read(_sockfd, &read_buf[0], MAX_BUFFER);
	if (rret == -1)
	{
		ZN_THROW(zn_cimap_read_exception, "");
		return false;
	}

	zn_logger::instance().debug() << "S: " << &read_buf[0] << zn_buffered_log::endl;

	const char *bufp = _options[CIMAP_LITERAL].c_str();
	size_t buf_len = _options[CIMAP_LITERAL].length();
	zn_logger::instance().debug() << "C: " << bufp << zn_buffered_log::endl;
	while (1)
	{
		ret = write(_sockfd, bufp, buf_len);
		if (ret == 0)
		{
			return false;
		}
		else if (ret < buf_len)
		{
			bufp += ret;
			buf_len -= ret;
		}
		else // (ret == buf_len)
		{
			break;
		}
	}
	ret = write(_sockfd, "\r\n", 2);

	rret = read(_sockfd, &read_buf[0], MAX_BUFFER);
	if (rret == -1)
	{
		ZN_THROW(zn_cimap_read_exception, "");
		return false;
	}

	zn_logger::instance().debug() << "S: " << &read_buf[0] << zn_buffered_log::endl;

	return _check_response(&read_buf[0]);
}

// zn_cimap_session
bool zn_cimap_session::init(const std::string &host, const std::string &port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;        /* Allow IPv4 only */
	hints.ai_socktype = SOCK_STREAM ; /* Datagram socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0;            /* Any protocol */

	int ret = getaddrinfo(host.c_str(), port.c_str(), &hints, &result);
	if (ret != 0)
	{
		ZN_THROW(zn_cimap_connect_failed_exception, "");
		return false;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		_sockfd = socket(rp->ai_family, rp->ai_socktype,
				rp->ai_protocol);
		if (_sockfd == -1)
			continue;

		if (connect(_sockfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;                  /* Success */

		close(_sockfd);
	}

	if (rp == NULL) {               /* No address succeeded */
		ZN_THROW(zn_cimap_connect_failed_exception, "");
		return false;
	}

	freeaddrinfo(result);           /* No longer needed */

	return true;
}

bool zn_nsim_session::nsim_login(const std::string &username, const std::string &password)
{
	// welcome
	zn_cimap_interaction_p welcome_cmd = _imap_session.get_cmd<zn_cimap_cmd_welcome>();
	welcome_cmd->perform();

	// login
	zn_cimap_interaction_p login_cmd = _imap_session.get_cmd<zn_cimap_cmd_login>();
	login_cmd->set_option(CIMAP_USERNAME, username).
		set_option(CIMAP_PASSWORD, password).
		perform();

	return true;
}

/*
static const char *mail_content = "Return-Path: null\r\n"
"Received: from localhost (LHLO izmbox01.ezweb.ne.jp) (127.0.0.1) by\r\n"
" izmbox01.ezweb.ne.jp with LMTP; Wed, 13 Aug 2014 17:02:04 +0900 (JST)\r\n"
"From: <juan-jie.xi@ezweb.ne.jp>\r\n"
"To: <koji.shimamura@ezweb.ne.jp>\r\n"
"Subject: Second mail for Zimbra Data Migration\r\n"
"Message-ID: <20140813165509159454@nxdir01.ezweb.ne.jp>\r\n"
"Date: Wed, 13 Aug 2014 16:55:09 +0900 (JST)\r\n"
"MIME-Version: 1.0\r\n"
"Content-Type: text/plain; charset=\"iso-2022-jp\"\r\n"
"Content-Transfer-Encoding: 7bit\r\n"
"\r\n"
"This is an email for Mr.Xi.\r\n"
"This is second mail.\r\n";

int main()
{
	zn_cimap_session imap_session;
	imap_session.init("15.210.144.146", "7143");

	// welcome
	zn_cimap_interaction_p welcome_cmd = imap_session.get_cmd<zn_cimap_cmd_welcome>();
	welcome_cmd->perform();

	// login
	zn_cimap_interaction_p login_cmd = imap_session.get_cmd<zn_cimap_cmd_login>();
	login_cmd->set_option(CIMAP_USERNAME, "juan-jie.xi@ezweb.ne.jp").
		set_option(CIMAP_PASSWORD, "password").
		perform();

	// create
	zn_cimap_interaction_p create_cmd = imap_session.get_cmd<zn_cimap_cmd_create>();
	create_cmd->set_option(CIMAP_MAILBOXNAME, "/Index/TTTTT").perform();

	// append
	zn_cimap_interaction_p append_cmd = imap_session.get_cmd<zn_cimap_cmd_append>();
	append_cmd->set_option(CIMAP_MAILBOXNAME, "/Index/TTTTT").
		set_option(CIMAP_FLAGS, "(\\Seen)").
		set_option(CIMAP_DATETIME, "").
		set_option(CIMAP_LITERAL, mail_content).
		perform();

	// logout
	zn_cimap_interaction_p logout_cmd = imap_session.get_cmd<zn_cimap_cmd_logout>();
	logout_cmd->perform();

	return 0;
}
*/
