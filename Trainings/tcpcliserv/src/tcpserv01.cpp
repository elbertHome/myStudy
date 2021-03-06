/*
 * tcpserv01.cpp
 *
 *  Created on: Dec 1, 2015
 *      Author: elbert
 */
#include "unp.h"
#include "sigchldwait.hpp"

int main(int argc, char ** argv)
{
	int listenfd;
	int connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr;
	struct sockaddr_in servaddr;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
	Listen(listenfd, LISTENQ);

	Signal(SIGCHLD, sig_chld);
	while (true)
	{
		clilen = sizeof(cliaddr);

		if ((connfd = Accept(listenfd, (SA *) &cliaddr, &clilen)) < 0)
		{
			if (errno == EINTR)
			{
				continue;
			}
			else
			{
				err_sys("accept error");
			}
		}


		// Child process
		if ((childpid = fork()) == 0)
		{
			Close(listenfd);
			str_echo(connfd);
			exit(0);
		}

		Close(connfd);
	}
}


