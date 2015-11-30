/*
 * daytimetcpcli.cpp
 *
 *  Created on: Jun 26, 2015
 *      Author: elbert
 */
#include "unp.h"

int main(int argc, char ** argv)
{
	// 客户端套接字
	int sockfd;
	// 套接字读取字串长度
	int n;
	// 套接字读取Buff
	char recvline[MAXLINE + 1];
	// 套接字地址结构体（ipv4）
	sockaddr_in servaddr;
	/* sockaddr_in6 servaddr;   //ipv6 */

	// 参数个数check
	if (argc != 2)
	{
		err_quit("usage: a.out <IPAddress>");
	}

	// 创建客户端套接字
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	/* sockfd = Socket(AF_INET6, SOCK_STREAM, 0);   //ipv6 */
	// 地址结构体初始化
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(13);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
	/* servaddr.sin6_family = AF_INET6;			//ipv6
	 * servaddr.sin6_port = htons(13);			//ipv6
	 * Inet_pton(AF_INET6, argv[1], &servaddr.sin6_addr);	//ipv6
	 * */

	// 服务端连接
	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

	// 循环读取服务端
	while ((n = Read(sockfd, recvline, MAXLINE)) > 0)
	{
		recvline[n] = 0;
		Fputs(recvline,stdout);
	}

	// 正常退出
	exit(EXIT_SUCCESS);
}

