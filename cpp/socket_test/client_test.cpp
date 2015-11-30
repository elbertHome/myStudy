/*
 * client_test.cpp
 *
 *  Created on: Dec 30, 2014
 *      Author: elbert
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

const static unsigned long MAXLEN = 1024;
int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in address;
	char char_send[MAXLEN];
	char char_recv[MAXLEN];


	if (argc != 3)
	{
		// Usage: fileclient <address> <port>
		printf("Usage: fileclient <address> <port>/n"); //用法：文件名 服务器IP地址 服务器端口地址
		return 1;
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
		exit(1);
	}

	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(atoi(argv[2]));
	if (inet_pton(AF_INET, argv[1], &address.sin_addr) <= 0)
	{
		printf("inet_pton error for %s\n", argv[1]);
		exit(1);
	}
	// connect
	if (connect(sockfd, (struct sockaddr*) &address, sizeof(address)) < 0)
	{
		printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
		exit(1);
	}

	while (true)
	{
		memset(char_send, 0, MAXLEN);
		memset(char_recv, 0, MAXLEN);
		// input send data
		scanf("%s", char_send);
		// flush
		fflush(stdin);
		// send data to server
		if (send(sockfd, char_send, strlen(char_send), 0) < 0)
		{
			printf("sending error: %s(errno: %d)\n", strerror(errno), errno);
			exit(1);
		}
		// exit when input exit
		if (strcmp(char_send, "exit") == 0)
		{
			printf("disconnect from server\n");
			break;
		}
		// recevie response from server
		if (recv(sockfd, char_recv, MAXLEN, 0) == -1)
		{
			printf("recevie error: %s(errno: %d)\n", strerror(errno), errno);
			exit(1);
		}
		printf("%s\n",char_recv);
	}
	close(sockfd);
	exit(0);
}

