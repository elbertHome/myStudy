#include <iostream>
#include <string>
#include <exception>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "zn_config_manager.hpp"
#include "zn_exception.hpp"
#include "zn_logger.hpp"
#include "zn_misc.hpp"
#include "zn_server.hpp"

extern bool is_child;

void zn_server::on_accept(evutil_socket_t fd)
{
	unsigned int max_conn_count = zn_config_manager::instance().get_long("TOOLSERV_LITSEN_COUNT");
	pid_t pid;

	if (_conn_count >= max_conn_count)
	{
		printf("over max connection.\n");
		close(fd);
		return;
	}

	if ((pid = fork()) == -1)
	{
		printf("fork error.\n");
		close(fd);
		return;
	}

	if (pid == 0)
	{
		event_reinit(_base);
		evsignal_del(_sigchld_event);
		evsignal_del(_sigint_event);
		evconnlistener_free(_listener);
		_listener = NULL;
		is_child = true;

		_conn = new zn_connection(fd, _base);
		_conn->start();
	}
	else
	{
		_conn_count++;
		//printf("current connection count is %d.\n", _conn_count);
		close(fd);
	}
}

void zn_server::listener_cb(struct evconnlistener *listener,
		evutil_socket_t fd, struct sockaddr *sa, int socklen, void *arg)
{
	zn_server *inst_ptr = reinterpret_cast<zn_server*>(arg);
	inst_ptr->on_accept(fd);
}

void zn_server::error_cb(struct evconnlistener *listener, void *arg)
{
	struct event_base *base = evconnlistener_get_base(listener);
	int err = EVUTIL_SOCKET_ERROR();
	fprintf(stderr, "Got an error %d (%s) on the listener. "
			"Shutting down./n", err, evutil_socket_error_to_string(err));

	event_base_loopexit(base, NULL);
}

void zn_server::sigchld_handler()
{
	pid_t	pid;
	int		stat;
	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
	{
		_conn_count--;
	}

	if (_end_flag && _conn_count == 0)
	{
		event_base_loopexit(_base, NULL);
	}
}

void zn_server::stop()
{
	if (_listener != NULL)
	{
		evconnlistener_free(_listener);
		_listener = NULL;
	}

	if (_conn_count == 0)
	{
		event_base_loopexit(_base, NULL);
	}

	_end_flag = true;
}

void zn_server::signal_cb(evutil_socket_t sig, short event, void *arg)
{
	zn_server *inst_ptr = reinterpret_cast<zn_server*>(arg);
	//printf("%s: got signal %d\n", __func__, sig);
	if (sig == SIGCHLD)
	{
		inst_ptr->sigchld_handler();
	}
	else if (sig == SIGINT)
	{
		inst_ptr->stop();
	}
}

void zn_server::discard_cb(int severity, const char *msg)
{
    /* This callback does nothing. */
	printf("%s: libevent log: %s\n", __func__, msg);
}

zn_server::~zn_server()
{
	if (_sigchld_event != NULL)
		event_free(_sigchld_event);
	if (_sigint_event != NULL)
		event_free(_sigint_event);
	if (_listener != NULL)
		evconnlistener_free(_listener);
	if (_base != NULL)
		event_base_free(_base);
	if (_conn != NULL)
		delete _conn;
}

void zn_server::run()
{
	unsigned short port = zn_config_manager::instance().get_long("TOOLSERV_PORT_NO");
	struct sockaddr_in sin;

	_base = event_base_new();
	if (_base == NULL)
	{
		ZN_THROW(zn_syserr_exception, "event_base_new()");
	}

	_sigchld_event = evsignal_new(_base, SIGCHLD, signal_cb, (void *)this);
	if (_sigchld_event == NULL || event_add(_sigchld_event, NULL) < 0)
	{
		ZN_THROW(zn_syserr_exception, "Could not create or add a signal event!");
	}

	_sigint_event = evsignal_new(_base, SIGINT, signal_cb, (void *)this);
	if (_sigint_event == NULL || event_add(_sigint_event, NULL) < 0)
	{
		ZN_THROW(zn_syserr_exception, "Could not create or add a signal event!");
	}

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(port);//5678

	_listener = evconnlistener_new_bind(_base, listener_cb, (void *)this,
			LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
			(struct sockaddr*)&sin,
			sizeof(sin));

	if (_listener == NULL)
	{
		ZN_THROW(zn_syserr_exception, "Could not create a listener!");
	}
	evconnlistener_set_error_cb(_listener, error_cb);

	event_set_log_callback(discard_cb);

	event_base_dispatch(_base);
}
