/*
 * sigchldwait.cpp
 *
 *  Created on: Dec 4, 2015
 *      Author: elbert
 */

#include "unp.h"
#include "sigchldwait.hpp"

void sig_chld(int signo)
{
	pid_t 	pid;
	int	    stat = 0;

	while( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
	{
		printf("child %d terminated. \n", pid);
	}

	return;
}




