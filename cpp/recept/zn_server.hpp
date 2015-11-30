#pragma once

#include <string>
#include <map>

#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/event.h>

#include "zn_connection.hpp"

class zn_server
{
	public:
		zn_server():
			_base(NULL),
			_sigchld_event(NULL),
			_sigint_event(NULL),
			_listener(NULL),
			_conn_count(0),
			_conn(NULL),
			_end_flag(false)
		{}
		~zn_server();
		void run();
		void stop();
		void sigchld_handler();
		void on_accept(evutil_socket_t fd);
		static void signal_cb(evutil_socket_t sig, short event, void *arg);
		static void listener_cb(struct evconnlistener *listener,
				evutil_socket_t fd, struct sockaddr *sa, int socklen, void *arg);
		static void error_cb(struct evconnlistener *listener, void *arg);
		static void discard_cb(int severity, const char *msg);

	private:
		struct event_base *_base;
		struct event *_sigchld_event;
		struct event *_sigint_event;
		struct evconnlistener *_listener;
		unsigned int _conn_count;
		zn_connection *_conn;
		bool _end_flag;
};
