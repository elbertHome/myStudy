/*
 * socketclient.cpp
 *
 *  Created on: Jan 6, 2015
 *      Author: elbert
 */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <string>
#include <cctype>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

bool findCRLF(const std::string & str);

const static unsigned long MAXLEN = 1024;
int main(int argc, char *argv[])
{
	using namespace std;
	int sockfd;
	struct sockaddr_in address;
	char char_send[MAXLEN];
	char char_recv[MAXLEN];
	int rcvRtn;
	bool loopCond = false;
	string strTmp;
	char chr;

	if (argc != 3)
	{
		// Usage: fileclient <address> <port>
		cout << "Usage: fileclient <address> <port>" << endl;
		exit(EXIT_FAILURE);
	}

	cout << "char # will be transfer as <CR>" << endl;
	cout << "char $ will be transfer as <LF>" << endl;
	cout << "socket string which before #$(<CRLF>) will be sent to server" << endl;

	// init socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		cout << "create socket error:" << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}

	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(atoi(argv[2]));
	if (inet_pton(AF_INET, argv[1], &address.sin_addr) <= 0)
	{
		cout << "inet_pton socket error:" << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}
	// connect
	if (connect(sockfd, (struct sockaddr*) &address, sizeof(address)) < 0)
	{
		cout << "connect socket error:" << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}

	while (true)
	{
		strTmp.clear();
		do
		{
			memset(char_recv, 0, MAXLEN);
			// recevie response from server
			rcvRtn = recv(sockfd, char_recv, MAXLEN - 1, 0);
			if (rcvRtn == -1) 			//receive error
			{
				cout << "socket received error:" << strerror(errno) << endl;
				exit(EXIT_FAILURE);
			}
			else if (rcvRtn == 0)		//disconnected
			{
				cout << "socket disconnected by server." << endl;
				exit(EXIT_FAILURE);
			}
			else						// receive OK
			{
				loopCond = !findCRLF(char_recv);
				strTmp.append(char_recv);
			}
		} while (loopCond);
		// output receive string
		cout << strTmp;

		strTmp.clear();
		do
		{
			memset(char_send, 0, MAXLEN);
			// input send data
			while (cin.get(chr))
			{
				if (chr == '\n' || chr == '\r')
				{
					break;
				}
				else if (chr == '#')
				{
					chr = '\r';
				}
				else if (chr == '$')
				{
					chr = '\n';
				}
				strTmp += chr;
			}

			loopCond = !findCRLF(strTmp);
			cout << "Totla length of intput string is " << strTmp.length() << endl;
		} while (loopCond);

		// send data to server
		if (send(sockfd, strTmp.c_str(), strTmp.length(), 0) < 0)
		{
			cout << "socket sent error:" << strerror(errno) << endl;
			exit(EXIT_FAILURE);
		}

	}
	close(sockfd);

	exit(EXIT_SUCCESS);
}

bool findCRLF(const std::string & str)
{
	bool rtn = false;
	for (unsigned int i = 0; i < str.length() - 1; i++)
	{
		if (str[i] == '\r' && str[i + 1] == '\n')
		{
			rtn = true;
			break;
		}
	}
	return rtn;

}

