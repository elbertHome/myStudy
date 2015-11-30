#include <string>
#include <sstream>

#include "zn_migration_queue.hpp"

#include "zn_misc.hpp"

#include "zn_exception.hpp"
#include "zn_retriable.hpp"
#include "zn_logger.hpp"
#include "zn_config_manager.hpp"

bool zn_migration_queue::init()
{
	_conn.set_option(new mysqlpp::MultiStatementsOption(true));
	_conn.set_option(new mysqlpp::ConnectTimeoutOption(_timeout));
	_conn.set_option(new mysqlpp::ReadTimeoutOption(_timeout));
	_conn.set_option(new mysqlpp::WriteTimeoutOption(_timeout));
	_conn.set_option(new mysqlpp::InteractiveOption(true));

	_conn.connect(_db_name.c_str(), _mysql_server.c_str(),
			_mysql_user.c_str(), _mysql_password.c_str(), _mysql_port);

	_conn.select_db(_db_name);

	return true;
}

bool zn_migration_queue::_excute_sql(const std::string &sql)
{
	mysqlpp::Query query = _conn.query();
	query << sql;

	return query.exec();
}

zn_migration_row zn_migration_queue::_excute_get_one_row(const std::string &sql)
{
	mysqlpp::Query query = _conn.query();
	query << sql;

	mysqlpp::StoreQueryResult res = query.store();
	if (res)
	{
		if (res.num_rows() > 1)
		{
			ZN_THROW(zn_migration_queue_exception, "More than one row found.", sql);
		}
		else if (res.num_rows() == 0)
		{
			ZN_THROW(zn_migration_queue_exception, "No data found.", sql);
		}
	}

	zn_migration_row ret;

	mysqlpp::Row row = res[0];

	row["LOGIN_ID"].to_string(ret.login_id);
	row["ZIMBRA_ID"].to_string(ret.zimbra_id);
	ret.node_id = row["NODE_ID"];
	row["EAS_HOST"].to_string(ret.eas_host);
	ret.proc_status = row["PROC_STATUS"];
	ret.imap_status = row["IMAP_STATUS"];
	ret.imap_retry_count = row["IMAP_RETRY_COUNT"];
	ret.imap_skip_count = row["IMAP_SKIP_COUNT"];
	ret.restart_count = row["RESTART_COUNT"];
	row["INSERT_TIME"].to_string(ret.insert_time);
	row["UPDATE_TIME"].to_string(ret.update_time);

	return ret;
}

// function only for test purpose
bool zn_migration_queue::insert_migration_information(const std::string &login_id, unsigned int node_id, std::string eas_host, unsigned int proc_status)
{
	std::ostringstream oss;
	oss << "INSERT INTO " << _table_name;
	oss << " (LOGIN_ID, NODE_ID, EAS_HOST, PROC_STATUS, INSERT_TIME)";
	oss << " VALUES (" <<
		"\"" << login_id << "\", " <<
		node_id << ", " <<
		"\"" << eas_host << "\", " <<
		zn_migration_queue::NOT_PROCESS << ", " <<
		"now())";

	return _excute_sql(oss.str());
}

zn_migration_row zn_migration_queue::get_migration_information(const std::string &login_id, bool recovery_flag)
{
	std::ostringstream oss;
	oss << "SELECT * FROM " << _table_name;
	oss << " WHERE LOGIN_ID = \"" << login_id << "\"";
	if (!recovery_flag)
	{
		oss << " AND PROC_STATUS <> " << zn_migration_queue::MIGRATION_BREAKOUT;
	}

	return _excute_get_one_row(oss.str());
}

bool zn_migration_queue::update_zimbra_id(const std::string &login_id, const std::string &zimbra_id)
{
	std::ostringstream oss;
	oss << "UPDATE " << _table_name;
	oss << " SET ZIMBRA_ID = \"" << zimbra_id << "\"";
	oss << " WHERE LOGIN_ID = \"" << login_id << "\"";

	return _excute_sql(oss.str());
}

bool zn_migration_queue::update_imap_status(const std::string &login_id, unsigned int imap_status)
{
	std::ostringstream oss;
	oss << "UPDATE " << _table_name;
	oss << " SET IMAP_STATUS = \"" << imap_status << "\"";
	oss << " WHERE LOGIN_ID = \"" << login_id << "\"";

	return _excute_sql(oss.str());
}

bool zn_migration_queue::update_proc_status(const std::string &login_id, unsigned int proc_status)
{
	std::ostringstream oss;
	oss << "UPDATE " << _table_name;
	oss << " SET PROC_STATUS = \"" << proc_status << "\"";
	oss << " WHERE LOGIN_ID = \"" << login_id << "\"";

	return _excute_sql(oss.str());
}

bool zn_migration_queue::_increment_count(const std::string &column_name, const std::string &login_id)
{
	std::ostringstream oss;
	oss << "UPDATE " << _table_name;
	oss << " SET IMAP_RETRY_COUNT = " << column_name << " + 1";
	oss << " WHERE LOGIN_ID = \"" << login_id << "\"";

	return _excute_sql(oss.str());
}

bool zn_migration_queue::increment_retry_count(const std::string &login_id)
{
	return _increment_count("IMAP_RETRY_COUNT", login_id);
}

bool zn_migration_queue::increment_skip_count(const std::string &login_id)
{
	return _increment_count("IMAP_SKIP_COUNT", login_id);
}

bool zn_migration_queue::increment_restart_count(const std::string &login_id)
{
	return _increment_count("RESTART_COUNT", login_id);
}

bool zn_migration_queue::delete_data(const std::string &login_id)
{
	std::ostringstream oss;
	oss << "DELETE FROM " << _table_name;
	oss << " WHERE LOGIN_ID = \"" << login_id << "\"";

	return _excute_sql(oss.str());
}

unsigned int zn_migration_queue::get_record_count(const std::string &condition)
{
	unsigned int count = 0;

	mysqlpp::Query query = _conn.query();
	query << "SELECT COUNT(*) FROM " << _table_name;
	if (condition.size() > 0)
	{
		query << " WHERE " << condition;
	}

	mysqlpp::StoreQueryResult res = query.store();
	mysqlpp::Row row = res[0];
	count = row["COUNT(*)"];

	return count;
}
