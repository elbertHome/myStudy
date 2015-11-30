#include <iostream>
#include <string>
#include <exception>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <mysqld_error.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include "zn_retcode.hpp"
#include "zn_config_manager.hpp"
#include "zn_exception.hpp"
#include "zn_logger.hpp"
#include "zn_misc.hpp"
#include "zn_dbserv.hpp"

zn_dbserv::zn_dbserv(int *channel):
	_conn(true),
	_base(NULL),
	_bev(NULL),
	_channel(channel[1])
{
	_mysql_server = zn_config_manager::instance().get("mysql_server");
	_mysql_port = from_string<unsigned int>(zn_config_manager::instance().get("mysql_port"));
	_mysql_user = zn_config_manager::instance().get("mysql_user");
	_mysql_password = zn_config_manager::instance().get("mysql_password");
	_db_name = zn_config_manager::instance().get("db_name");
	_table_name = zn_config_manager::instance().get("table_name");
	_table_name = zn_config_manager::instance().get("DB_TABLE_NAME");
	_timeout = from_string<unsigned int>(zn_config_manager::instance().get("timeout"));

	_toolserv_transact_max = from_string<unsigned int>(zn_config_manager::instance().get("TOOLSERV_TRANSACT_MAX"));;
	_toolserv_export_max = from_string<unsigned int>(zn_config_manager::instance().get("TOOLSERV_EXPORT_MAX"));;
	_mbs_connect_max = from_string<unsigned int>(zn_config_manager::instance().get("MBS_CONNECT_MAX"));;

	close(channel[0]);
}

int zn_dbserv::connect_db()
{
	_conn = mysqlpp::Connection(true);
	_conn.set_option(new mysqlpp::MultiStatementsOption(true));
	_conn.set_option(new mysqlpp::ConnectTimeoutOption(_timeout));
	_conn.set_option(new mysqlpp::ReadTimeoutOption(_timeout));
	_conn.set_option(new mysqlpp::WriteTimeoutOption(_timeout));
	_conn.set_option(new mysqlpp::InteractiveOption(true));

	_conn.connect(_db_name.c_str(), _mysql_server.c_str(),
			_mysql_user.c_str(), _mysql_password.c_str(), _mysql_port);

	_conn.select_db(_db_name);

	return 0;
}

unsigned int zn_dbserv::select_count(const std::string &condition)
{
	unsigned int count = 0;

	mysqlpp::Query query = _conn.query();
	query << "SELECT COUNT(*) FROM " << _table_name;
	if (condition.size() > 0)
	{
		query << " WHERE " << condition;
	}
	std::cout << query << std::endl;
	mysqlpp::StoreQueryResult res = query.store();
	mysqlpp::Row row = res[0];
	count = row["COUNT(*)"];

	std::cout << "select_count: " << count << std::endl;
	return count;
}

bool zn_dbserv::insert(std::string &login_id, unsigned int node_id, std::string &eas_host)
{
	mysqlpp::Query query = _conn.query();
	query << "INSERT INTO " << _table_name <<
			" (LOGIN_ID, NODE_ID, EAS_HOST, PROC_STATUS, INSERT_TIME)" <<
			" VALUES (" <<
			"\"" << login_id << "\", " <<
			node_id << ", " <<
			"\"" << eas_host << "\", " <<
			0 << ", " <<
			"NOW())";

	return query.exec();
}

int zn_dbserv::access_db(std::string &login_id, unsigned int node_id, std::string &eas_host)
{
	std::string condition;

	try
	{
		condition = "";
		if (select_count(condition) > _toolserv_transact_max)
		{
			return RC_SERV_BUSY;
		}

		condition = "PROC_STATUS = 0 OR PROC_STATUS = 1";
		if (select_count(condition) > _toolserv_export_max)
		{
			return RC_SERV_BUSY;
		}

		condition = "PROC_STATUS = 0 OR PROC_STATUS = 1 AND EAS_HOST = \"" + eas_host + "\"";
		if (select_count(condition) > _mbs_connect_max)
		{
			return RC_EAS_BUSY;
		}

		insert(login_id, node_id, eas_host);
	}
	catch (mysqlpp::BadQuery &e)
	{
		std::cerr << "Query exception: " << e.what() << std::endl;
		if (e.errnum() == ER_DUP_ENTRY)
		{
			return RC_EXIST;
		}
		return RC_DB_ERR;
	}
	catch (mysqlpp::Exception &e)
	{
		std::cerr << "General exception: " << e.what() << std::endl;
		return RC_DB_ERR;
	}

	return 0;
}

void zn_dbserv::access_main_process(std::string fd, std::string ret_code, std::string login_id)
{
	struct evbuffer *output;
	std::string rep_line = fd + " " + ret_code + " " + login_id + "\r\n";

	output = bufferevent_get_output(_bev);
	evbuffer_add(output, rep_line.c_str(), rep_line.size());
}

void zn_dbserv::on_read()
{
	struct evbuffer *input;
	char *line;
	size_t n;
	int ret;
	input = bufferevent_get_input(_bev);

	while ((line = evbuffer_readln(input, &n, EVBUFFER_EOL_CRLF_STRICT)))
	{
		//fd login_id node_id eas_host
		printf("DBSERV------------ RECV: %s\n", line);
		std::string fd;
		std::string login_id;
		std::string node_id;
		std::string eas_host;
		size_t pos = 0;

		get_word(line, pos, fd);
		get_word(line, pos, login_id);
		get_word(line, pos, node_id);
		get_word(line, pos, eas_host);

		//login_id = "99999999999999";
		//node_id = "150";
		//eas_host = "zm03box01.ezweb.ne.jp";

		ret = access_db(login_id, from_string<unsigned int>(node_id), eas_host);
		access_main_process(fd, to_string(ret), login_id);

		free(line);
	}
}

void zn_dbserv::read_cb(struct bufferevent *bev, void *arg)
{
	zn_dbserv *inst_ptr = reinterpret_cast<zn_dbserv*>(arg);
	inst_ptr->on_read();
}

void zn_dbserv::error_cb(struct bufferevent *bev, short error, void *ctx)
{
	printf("DBSV ERROR: %s\n", "callback");
	bufferevent_free(bev);
}

void zn_dbserv::run()
{
	connect_db();

	_base = event_base_new();
	if (_base == NULL)
	{
		ZN_THROW(zn_syserr_exception, "event_base_new()");
	}

	evutil_make_socket_nonblocking(_channel);
	_bev = bufferevent_socket_new(_base, _channel, BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(_bev, read_cb, NULL, error_cb, (void*)this);
	bufferevent_enable(_bev, EV_READ|EV_WRITE);

	event_base_dispatch(_base);
}

void zn_dbserv::dbserv_main(int *channel)
{
	try
	{
		printf("dbserv_main------------ start: %d\n", 1);
		zn_dbserv dbsv(channel);
		dbsv.run();
	}
	catch(const zn_exception &e)
	{
		zn_logger::instance().error(e.what());
	}
	catch (const std::exception &e)
	{
		std::cerr << "Unexpected exception: " << e.what() << std::endl;
	}

	return;
}

