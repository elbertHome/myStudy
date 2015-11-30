#pragma once

#include <string>
#include <map>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include "zn_result_code.hpp"

class zn_connection
{
	public:
		zn_connection(evutil_socket_t fd, struct event_base *base):
			_base(base),
			_bev(NULL),
			_fd(fd),
			_close_flag(false)
		{}
		void start();
		void end(short event);
		int check_request(const std::string &request);
		int check_prevent_file(const std::string &eas_host);
		int check_prevent_file();
		int access_db();
		int execute(const char *cmd, const char *arg);
		int execute();
		std::string &make_response(zn_result_code rc);
		void do_reply(zn_result_code rc);
		void on_read();
		void on_write();

		static void read_cb(struct bufferevent *bev, void *arg);
		static void write_cb(struct bufferevent *bev, void *arg);
		static void event_cb(struct bufferevent *bev, short error, void *arg);

	private:
		struct event_base *_base;
		struct bufferevent *_bev;
		evutil_socket_t _fd;
		std::string _login_id;
		std::string _node_id;
		std::string _eas_host;
		std::string _response;
		bool _close_flag;
};
