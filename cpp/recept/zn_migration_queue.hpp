#pragma once

#include <string>

#include <mysql++/mysql++.h>

class zn_migration_row;

class zn_migration_queue
{
	private:
		std::string _mysql_server;
		unsigned int _mysql_port;
		std::string _mysql_user;
		std::string _mysql_password;
		std::string _db_name;
		std::string _table_name;
		unsigned int _timeout;

		mysqlpp::Connection _conn;

		bool _excute_sql(const std::string &sql);
		zn_migration_row _excute_get_one_row(const std::string &sql);

		bool _increment_count(const std::string &column_name, const std::string &login_id);

	public:
		static const unsigned int NOT_PROCESS = 0;
		static const unsigned int EXPORTING = 1;
		static const unsigned int ANALYZING = 2;
		static const unsigned int MIGRATING = 3;
		static const unsigned int MIGRATION_BREAKOUT = 4;

		zn_migration_queue(const std::string &mysql_server,
				unsigned int mysql_port,
				const std::string &mysql_user,
				const std::string &mysql_password,
				const std::string &db_name,
				const std::string &table_name,
				unsigned int timeout = 10):
			_mysql_server(mysql_server),
			_mysql_port(mysql_port),
			_mysql_user(mysql_user),
			_mysql_password(mysql_password),
			_db_name(db_name),
			_table_name(table_name),
			_timeout(timeout),
			_conn(true)
		{}

		~zn_migration_queue()
		{}

		bool init();
		bool insert_migration_information(const std::string &login_id, unsigned int node_id, std::string eas_host, unsigned int proc_status);
		zn_migration_row get_migration_information(const std::string &login_id, bool recovery_flag = true);
		bool update_zimbra_id(const std::string &login_id, const std::string &zimbra_id);
		bool update_imap_status(const std::string &login_id, unsigned int imap_status);
		bool update_proc_status(const std::string &login_id, unsigned int proc_status);
		inline bool increment_retry_count(const std::string &login_id);
		inline bool increment_skip_count(const std::string &login_id);
		inline bool increment_restart_count(const std::string &login_id);
		bool delete_data(const std::string &login_id);
		unsigned int get_record_count(const std::string &condition);
};

class zn_migration_row
{
	public:
		std::string login_id;
		std::string zimbra_id;
		unsigned int node_id;
		std::string eas_host;
		unsigned int proc_status;
		unsigned int imap_status;
		unsigned int imap_retry_count;
		unsigned int imap_skip_count;
		unsigned int restart_count;
		std::string insert_time;
		std::string update_time;

		zn_migration_row():
			node_id(0),
			proc_status(zn_migration_queue::NOT_PROCESS),
			imap_status(zn_migration_queue::NOT_PROCESS),
			imap_retry_count(0),
			imap_skip_count(0),
			restart_count(0)
		{}
};
