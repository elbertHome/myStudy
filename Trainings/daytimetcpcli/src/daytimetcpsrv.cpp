/*
 * daytimetcpsrv.cpp
 *
 *  Created on: Jun 28, 2015
 *      Author: elbert
 */
#include "unp.h"

int main(int argc, char ** argv)
{
	// 监听套接字
	int listenfd;
	// 连接套接字
	int connfd;
	// 服务端地址结构体
	sockaddr_in servaddr;
	// 发送字符BUFF
	char buff[MAXLINE];
	// 系统时间
	time_t ticks;

	// 创建监听套接字
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	// 初始化服务端地址结构体
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(13);

	// 绑定地址到监听套接字
	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	// 监听开始，监听最大数由LISTENQ决定，客户端以单线程队列访问服务端
	Listen(listenfd, LISTENQ);

	// 循环等待，直到服务端被终止（kill）
	while (true)
	{
		// 取得连接套接字（客户端服务端已经套接）
		connfd = Accept(listenfd, (SA *) nullptr, nullptr);

		// 获取一定格式的系统时间
		ticks = time(nullptr);
		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
		// 发送给客户端
		Write(connfd, buff, strlen(buff));
		// 关闭连接
		Close(connfd);
	}

	// 正常终了退出
	exit(EXIT_SUCCESS);
}

