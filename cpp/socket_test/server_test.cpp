/*
 * server_test.cpp
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
#include <pthread.h>
#include <string.h>
#include <errno.h>

const static unsigned long PORT = 8888;
const static unsigned long MAXLEN = 1024;

void *rec_data(void *fd);
int main(int argc, char *argv[])
{
	int server_sockfd;
	int *client_sockfd = (int *) malloc(sizeof(int));
	struct sockaddr_in server_address;

	//init socket
	if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
		exit(1);
	}
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	// set host IP address
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	// set port
	server_address.sin_port = htons(PORT);

	// bind address to socket
	if (bind(server_sockfd, (struct sockaddr*) &server_address, sizeof(server_address)) == -1)
	{
		printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
		exit(1);
	}

	// listen beginning
	if (listen(server_sockfd, 10) == -1)
	{
		printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
		exit(1);
	}
	printf("======waiting for client's request======\n");

	while (true)
	{
		pthread_t thread;
		// socket accept
		*client_sockfd = accept(server_sockfd, (struct sockaddr *) NULL, NULL);
		if (-1 == *client_sockfd)
		{
			printf("accept error: %s(errno: %d)\n", strerror(errno), errno);
			continue;
		}
		printf("======server socket accepted======\n");
		// create sub process to receive request
		if (pthread_create(&thread, NULL, rec_data, client_sockfd) != 0)
		{
			printf("thread create error: %s(errno: %d)\n", strerror(errno), errno);
			break;
		}
	}
	shutdown(*client_sockfd, 2);
	shutdown(server_sockfd, 2);
	free(client_sockfd);
	exit(0);
}
/*****************************************
 * 函数名称：rec_data
 * 功能描述：接受客户端的数据
 * 参数列表：fd——连接套接字
 * 返回结果：void
 *****************************************/
void *rec_data(void *fd)
{
	int client_sockfd;
	// recevie buff
	char recv_buff[MAXLEN];
	// send buff
	char send_buff[MAXLEN];
	char start[5];

	client_sockfd = *((int*) fd);
	// sub process start
	printf("======sub process started======\n");
	while (true)
	{
		memset(recv_buff, 0, MAXLEN);
		memset(send_buff, 0, MAXLEN);
		memset(start, 0, 5);
		if (recv(client_sockfd, recv_buff, MAXLEN, 0) == -1)
		{
			printf("recevie error: %s(errno: %d)\n", strerror(errno), errno);
			exit(1);
		}

		printf("receive from client is:%s", recv_buff);
		fflush(stdout);
		strncpy(start, recv_buff, 4);
		// exit when received exit
		if (strcmp(start, "exit") == 0)
		{
			printf("======client has disconnected======\n");
			break;
		}

		sprintf(send_buff, "server has recevied:%s", recv_buff);
		if (send(client_sockfd, send_buff, strlen(send_buff), 0) == -1)
		{
			printf("send error: %s(errno: %d)\n", strerror(errno), errno);
			exit(1);
		}
	}
	printf("======sub process closed======\n");
	close(client_sockfd);
	pthread_exit(0);
}

