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
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <mysqld_error.h>

#include "zn_migration_queue.hpp"
#include "zn_config_manager.hpp"
#include "zn_xml.hpp"
#include "zn_exception.hpp"
#include "zn_logger.hpp"
#include "zn_misc.hpp"
#include "zn_connection.hpp"

int zn_connection::check_request(const std::string &request)
{
	unsigned int login_id_max = zn_config_manager::instance().get_long("REQUEST_LOGIN_ID_MAX");
	unsigned int node_id_max = zn_config_manager::instance().get_long("REQUEST_NODE_ID_MAX");
	unsigned int eas_host_max = zn_config_manager::instance().get_long("REQUEST_EAS_HOST_MAX");

	size_t pos = 0;
	std::size_t i;

	if (request.empty())
	{
		return RC_FORMAT_ERR;
	}

	// ログイン名取得とチェック
	get_word(request, pos, _login_id);
	if (_login_id.empty() || _login_id.length() > login_id_max)
	{
		return RC_FORMAT_ERR;
	}

	for(i = 0; i < _login_id.length(); i++)
	{
		if(!isalnum(_login_id[i]))
		{
			return RC_FORMAT_ERR;
		}
	}

	// 収容NSIMノード取得とチェック
	get_word(request, pos, _node_id);
	if (_node_id.empty() || _node_id.length() > node_id_max)
	{
		return RC_FORMAT_ERR;
	}

	for(i = 0; i < _node_id.length(); i++)
	{
		if(!isalnum(_node_id[i]))
		{
			return RC_FORMAT_ERR;
		}
	}

	// 収容EAS Mailbox(FQDN形式)取得とチェック
	get_word(request, pos, _eas_host);
	if (request.at(pos -1) == ' ')
	{
		return RC_FORMAT_ERR;
	}

	if (_eas_host.empty() || _eas_host.length() > eas_host_max)
	{
		return RC_FORMAT_ERR;
	}

	for(i = 0; i < _eas_host.size(); i++)
	{
		if(!isalnum(_eas_host[i])
			&& _eas_host[i] != '.'
			&& _eas_host[i] != '-')
		{
			return RC_FORMAT_ERR;
		}
	}

	return 0;
}

int zn_connection::check_prevent_file()
{
	std::string prevent_dir = zn_config_manager::instance().get("TRANS_PREVENT_DIR");
	std::string prevent_file4toolserver = zn_config_manager::instance().get("TRANS_PREVENT_FILE4TOOLSERVER");
	std::string lockflie;

	// 抑止ファイル格納ディレクトリがない
	if (access(prevent_dir.c_str(), F_OK) != 0)
	{
		return RC_SERV_ERR;
	}

	// ツールサーバ用抑止ファイルが存在する
	lockflie = prevent_dir + "/" + prevent_file4toolserver + ".lock";
	if (access(lockflie.c_str(), F_OK) == 0)
	{
		return RC_SERV_MAINT;
	}

	// EAS Mailbox用抑止ファイルが存在する
	lockflie = prevent_dir + "/" + _eas_host + ".lock";
	if (access(lockflie.c_str(), F_OK) == 0)
	{
		return RC_EAS_MAINT;
	}

	return 0;
}

int zn_connection::access_db()
{
	unsigned int transact_max = zn_config_manager::instance().get_long("TOOLSERV_TRANSACT_MAX");
	unsigned int export_max = zn_config_manager::instance().get_long("TOOLSERV_EXPORT_MAX");
	unsigned int connect_max = zn_config_manager::instance().get_long("MBS_CONNECT_MAX");

	zn_migration_queue migration_queue(
			zn_config_manager::instance().get("DB_HOST_NAME"),
			zn_config_manager::instance().get_long("DB_PORT_NO"),
			zn_config_manager::instance().get("DB_USER"),
			zn_xml::get_zimbra_mysql_passwd(readfile(zn_config_manager::instance().get("DB_PASSWORD_XML"))),
			zn_config_manager::instance().get("DB_NAME"),
			zn_config_manager::instance().get("DB_TABLE_NAME"));

	try
	{
		// 移行実施キューDB接続処理
		migration_queue.init();

		// 全レコード件数を取得し、ツールサーバ処理上限数と比較
		std::string condition = "";
		if (migration_queue.get_record_count(condition) > transact_max)
		{
			return RC_SERV_BUSY;
		}

		// 「未処理」または「エクスポート中」のレコード数を取得し、ツールサーバエクスポート上限数と比較
		condition = "PROC_STATUS = 0 OR PROC_STATUS = 1";
		if (migration_queue.get_record_count(condition) > export_max)
		{
			return RC_SERV_BUSY;
		}

		// 特定EAS Mailboxに対する「未処理」、「エクスポート中」のレコード数を取得し、同一EASMailbox接続上限数と比較
		condition = "PROC_STATUS = 0 OR PROC_STATUS = 1 AND EAS_HOST = \"" + _eas_host + "\"";
		if (migration_queue.get_record_count(condition) > connect_max)
		{
			return RC_EAS_BUSY;
		}

		// 移行開始要求電文内容を登録する
		migration_queue.insert_migration_information(
				_login_id, from_string<unsigned int>(_node_id), _eas_host, 0);
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

int zn_connection::execute()
{
	std::string msgmv_cmd = zn_config_manager::instance().get("MSGMV_CMD");
	std::string msgmv_conf = zn_config_manager::instance().get("MSGMV_CONF_PATH");

	int pid;

	if ((pid = fork()) == -1)
	{
		printf("fork error.\n");
		return RC_SERV_ERR;
	}

	if (pid == 0)
	{
		close(_fd);
		if ((pid = fork()) == -1)
		{
			printf("fork error.\n");
			exit(1);
		}

		if (pid == 0)
		{
			if (execlp(msgmv_cmd.c_str(), msgmv_cmd.c_str(),
					msgmv_conf.c_str(), _login_id.c_str(), (char*)0) < 0)
			{
				printf("execlp error.\n");
			}
		}
		exit(0);
	}
	else
	{
		wait(NULL);
	}

	return 0;
}

std::string &zn_connection::make_response(zn_result_code rc)
{
	std::string res_code = to_string(rc);
	std::string parameter_name = "RESPONSE_CHAR_";
	parameter_name += res_code;

	_response = res_code + " " + zn_config_manager::instance().get(parameter_name);

	return _response;
}

void zn_connection::do_reply(zn_result_code rc)
{
	// 応答電文を作成する
	std::string response = make_response(rc);
	response += "\r\n";

	// 応答電文をNSIMに送信する
	bufferevent_write(_bev, response.c_str(), response.length());
	_close_flag = true;
}

void zn_connection::on_read()
{
	struct timeval timeout = {INT_MAX, 0};
	struct evbuffer *input;
	char *request_line;
	size_t len;

	bufferevent_set_timeouts(_bev, &timeout, NULL);
	input = bufferevent_get_input(_bev);

	request_line = evbuffer_readln(input, &len, EVBUFFER_EOL_CRLF_STRICT);
	if (request_line != NULL)
	{
		int ret;

		// 移行開始要求電文をログ出力する
		zn_logger::instance().info("Request received. Request:" + to_string(request_line));

		do
		{
			// 移行開始要求電文をチェックする
			if ((ret = check_request(request_line)) != 0)
				break;

			// 移行抑止ファイルが存在するか確認する
			if ((ret = check_prevent_file()) != 0)
				break;

			// 移行実施数判断と移行実施キュー登録処理
			if ((ret = access_db()) != 0)
				break;

			// 移行実施機能を起動する
			if ((ret = execute()) != 0)
				break;

			ret = RC_ACCEPT;
		}
		while (0);

		// 移行受付応答を返却する
		do_reply((zn_result_code)ret);

		free(request_line);
	}
}

void zn_connection::on_write()
{
	if (_close_flag)
	{
		end(0);
	}
}

void zn_connection::end(short event)
{
	if (event == 0)
	{
		// 応答電文をログ出力する
		zn_logger::instance().info("Result responded. Response:" + _response);
	}
	else if (event & (BEV_EVENT_TIMEOUT | BEV_EVENT_READING))
	{
		// 移行開始要求タイムアウト場合をログ出力する
		zn_logger::instance().error("Request timeout.");
	}

	bufferevent_free(_bev);
	_bev = NULL;

	// コネクション終了ログを出力する
	zn_logger::instance().info("Session terminated.");
}

void zn_connection::read_cb(struct bufferevent *bev, void *arg)
{
	zn_connection *inst_ptr = reinterpret_cast<zn_connection*>(arg);
	inst_ptr->on_read();
}

void zn_connection::write_cb(struct bufferevent *bev, void *arg)
{
	zn_connection *inst_ptr = reinterpret_cast<zn_connection*>(arg);
	inst_ptr->on_write();
}

void zn_connection::event_cb(struct bufferevent *bev, short event, void *arg)
{
	zn_connection *inst_ptr = reinterpret_cast<zn_connection*>(arg);
	inst_ptr->end(event);
}

void zn_connection::start()
{
	std::string greeting_msg = zn_config_manager::instance().get("GREETING_CHAR") + "\r\n";
	unsigned int request_timer = zn_config_manager::instance().get_long("TRANS_REQUEST_TIMER");
	struct timeval timeout = {request_timer, 0};

	// コネクション確立ログを出力する
	zn_logger::instance().info("Session started.");

	evutil_make_socket_nonblocking(_fd);
	_bev = bufferevent_socket_new(_base, _fd, BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(_bev, read_cb, write_cb, event_cb, (void*)this);
	bufferevent_set_timeouts(_bev, &timeout, NULL);
	bufferevent_enable(_bev, EV_READ|EV_WRITE);

	// グリーティングを送信する
	bufferevent_write(_bev, greeting_msg.c_str(), greeting_msg.length());
}
