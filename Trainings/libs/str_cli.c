#include	"unp.h"

void
str_cli(FILE *fp, int sockfd)
{
	int 	maxfdp;
	int 	stdineof;
	fd_set 	rset;
	char	buf[MAXLINE];
	int 	n;

	stdineof = 0;
	FD_ZERO(&rset);

	while(1)
	{
		if (stdineof == 0)
		{
			FD_SET(fileno(fp), &rset);
		}
		FD_SET(sockfd, &rset);
		maxfdp = max(fileno(fp), sockfd) + 1;

		Select(maxfdp, &rset, NULL, NULL, NULL);

		// socket is readable
		if (FD_ISSET(sockfd, &rset))
		{
			if ( (n = Read(sockfd, buf, MAXLINE)) == 0)
			{
				if (stdineof == 1)
				{
					// socket was closed and file(input) was also closed
					// normal termination
					return;
				}
				else
				{
					err_quit("str_cli: server terminated prematurely.");
				}
			}
			Write(fileno(stdout), buf, n);
		}

		// file(input) is readable
		if (FD_ISSET(fileno(fp), &rset))
		{
			if ((n = Read(fileno(fp), buf, MAXLINE)) == 0)
			{
				stdineof = 1;					// set eof flag of the input
				Shutdown(sockfd, SHUT_WR);		// send FIN to server
				FD_CLR(fileno(fp), &rset);
				continue;
			}

			Write(sockfd, buf, n);
		}


	}



}
