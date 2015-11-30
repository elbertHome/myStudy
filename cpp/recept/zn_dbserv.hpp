#pragma once

#include <string>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <mysql++/mysql++.h>

class zn_dbserv
{
	public:
		zn_dbserv(int *channel);
		int connect_db();
		unsigned int select_count(const std::string &condition);
		bool insert(std::string &login_id, unsigned int node_id, std::string &eas_host);
		int access_db(std::string &login_id, unsigned int node_id, std::string &eas_host);
		void access_main_process(std::string fd, std::string ret_code, std::string login_id);
		void on_read();
		void run();

		static void read_cb(struct bufferevent *bev, void *ctx);
		static void error_cb(struct bufferevent *bev, short error, void *ctx);
		static void dbserv_main(int *channel);

	private:
		std::string _mysql_server;
		unsigned int _mysql_port;
		std::string _mysql_user;
		std::string _mysql_password;
		std::string _db_name;
		std::string _table_name;
		unsigned int _timeout;
		unsigned int _toolserv_transact_max;
		unsigned int _toolserv_export_max;
		unsigned int _mbs_connect_max;

		mysqlpp::Connection _conn;
		struct event_base *_base;
		struct bufferevent *_bev;
		int _channel;
};
