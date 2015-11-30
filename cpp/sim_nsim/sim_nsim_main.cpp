/*
 * sim_nsim_main.cpp
 *
 *  Created on: Dec 30, 2014
 *      Author: elbert
 */

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

#include "Configuration.hpp"
#include "commontools.hpp"

// static global const variable
static const unsigned int SOCKET_LINE_MAX = 1024;
static const unsigned int SOCKET_LISTEN_MAX = 10;

struct imapCommand
{
		std::string tag;
		std::string imapCmd;
		std::string para1;
		std::string Others;
};

// function protope
void socketMainThreadRun();
void *socketSubThreadRun(void *fd);
bool doRequest(int *fd, std::string& strRecv);
bool doResponse(int *fd, const char* recvData);
bool doSendBack(int *fd, const char* sendData);
imapCommand getImapCommand(const std::string& strCmd);

// using declaration
int main(int argc, char *argv[])
{
	using namespace std;
	basic_ostream<char> * pCurLog = &cout;
	try
	{
		// parameter check
		if (argc < 2)
		{
			throw(string("usage:sim_nsim <config path>\n"));
		}
		Configuration::initInstance(argv[1]);
		pCurLog = &(Configuration::getInstance()->getLog());
		*pCurLog << getLocalTimeAndPid() << "get config successfully" << endl;
		*pCurLog << getLocalTimeAndPid() << "function main start" << endl;

		socketMainThreadRun();

	}
	catch (const std::string &ex)
	{
		*pCurLog << getLocalTimeAndPid() << ex;
		*pCurLog << getLocalTimeAndPid() << "function main exit abnormally!!!!!!" << endl;
		exit(EXIT_FAILURE);
	}
	*pCurLog << getLocalTimeAndPid() << "function main exit normally" << endl;
	exit(EXIT_SUCCESS);
}

void socketMainThreadRun()
{
	using namespace std;
	ofstream &curlog = Configuration::getInstance()->getLog();
	int server_sockfd;
	int *client_sockfd = new int;
	struct sockaddr_in server_address;
	ostringstream ex;
	try
	{
		curlog << getLocalTimeAndPid() << "function socketMainThreadRun start" << endl;
		//init socket
		if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		{
			ex.flush();
			ex << "create socket error: " << strerror(errno) << "(errorno: " << errno << ")\n";
			throw(ex.str());
		}
		curlog << getLocalTimeAndPid() << "create socket successfuly" << endl;

		memset(&server_address, 0, sizeof(server_address));
		server_address.sin_family = AF_INET;
		// set host IP address
		server_address.sin_addr.s_addr = htonl(INADDR_ANY);
		// set port
		server_address.sin_port = htons(atoi(Configuration::getInstance()->getPort().c_str()));

		int option = 1;
		// set socket reuse
		if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) < 0)
		{
			ex.flush();
			ex << "set socket reuse: " << strerror(errno) << "(errorno: " << errno << ")\n";
			throw(ex.str());
		}

		// bind address to socket
		if (bind(server_sockfd, (struct sockaddr*) &server_address, sizeof(server_address)) == -1)
		{
			ex.flush();
			ex << "bind socket error: " << strerror(errno) << "(errorno: " << errno << ")\n";
			throw(ex.str());
		}
		curlog << getLocalTimeAndPid() << "bind socket successfuly" << endl;

		// listen beginning
		if (listen(server_sockfd, SOCKET_LISTEN_MAX) == -1)
		{
			ex.flush();
			ex << "lithen socket error: " << strerror(errno) << "(errorno: " << errno << ")\n";
			throw(ex.str());
		}
		curlog << getLocalTimeAndPid() << "socket listening successfuly, maxlistening count ="
				<< SOCKET_LISTEN_MAX << endl;
		curlog << getLocalTimeAndPid() << "waiting for client's request" << endl;

		while (true)
		{
			pthread_t thread;
			// socket accept
			*client_sockfd = accept(server_sockfd, (struct sockaddr *) NULL, NULL);
			if (-1 == *client_sockfd)
			{
				ex.flush();
				ex << "accept socket error: " << strerror(errno) << "(errorno: " << errno << ")\n";
				throw(ex.str());
			}
			curlog << getLocalTimeAndPid() << "server socket accepted" << endl;

			// create sub process to receive request
			if (pthread_create(&thread, NULL, socketSubThreadRun, client_sockfd) != 0)
			{
				ex.flush();
				ex << "thread create error: " << strerror(errno) << "(errorno: " << errno << ")\n";
				throw(ex.str());
			}
		}
	}
	catch (const string &exception)
	{
		shutdown(*client_sockfd, 2);
		shutdown(server_sockfd, 2);
		delete client_sockfd;
		curlog << getLocalTimeAndPid() << "function socketMainThreadRun exit abnormally!!!!"
				<< endl;
		throw exception;
	}

	shutdown(*client_sockfd, 2);
	shutdown(server_sockfd, 2);
	delete client_sockfd;
	curlog << getLocalTimeAndPid() << "function socketMainThreadRun exit normally" << endl;
}

void *socketSubThreadRun(void *fd)
{
	using namespace std;
	ofstream &curlog = Configuration::getInstance()->getLog();
	int client_sockfd = *((int*) fd);
	string strRecv;
	try
	{
		curlog << getLocalTimeAndPid(pthread_self()) << "function socketSubThreadRun start" << endl;
		// recevie string

		ostringstream ex;

		string strConnect = Configuration::getInstance()->getStrConnect();
		//send welcome to client
		if (!doSendBack(&client_sockfd, strConnect.c_str()))
		{
			ex.flush();
			ex << "welcome error" << endl;
			throw(ex.str());
		}
		// sub process start
		while (true)
		{
			strRecv.clear();
			// receive from client
			if (!doRequest(&client_sockfd, strRecv))
			{
				curlog << getLocalTimeAndPid(pthread_self()) << "Disconnect from Client" << endl;
				break;
			}

			cout << "RECV:" << strRecv << endl;

			// send response to client
			// process only (SOCKET_LINE_MAX - 1) length of str
			if (strRecv.length() > SOCKET_LINE_MAX - 1)
			{
				strRecv = strRecv.substr(0, SOCKET_LINE_MAX - 1);
			}

			if (!doResponse(&client_sockfd, strRecv.c_str()))
			{
				curlog << getLocalTimeAndPid(pthread_self()) << "Disconnet from Server" << endl;
				break;
			}
		}
	}
	catch (const string &exception)
	{
		close(client_sockfd);
		curlog << getLocalTimeAndPid(pthread_self())
				<< "function socketSubThreadRun exit abnormally!!!!" << endl;
		pthread_detach(pthread_self());
		pthread_exit(0);
	}
	close(client_sockfd);
	curlog << getLocalTimeAndPid(pthread_self()) << "function socketSubThreadRun exit normally"
			<< endl;
	pthread_detach(pthread_self());
	pthread_exit(0);
}

bool doRequest(int *fd, std::string & strRcv)
{
	using namespace std;
	ofstream &curlog = Configuration::getInstance()->getLog();
	bool funRtn = true;
	bool loopCond = false;
	int rcvRtn;
	char rcvTmp[SOCKET_LINE_MAX];
	string strTmp;
	curlog << getLocalTimeAndPid(pthread_self()) << "function doRequest start" << endl;
	do
	{
		memset(rcvTmp, 0, sizeof(rcvTmp));
		rcvRtn = recv(*fd, rcvTmp, SOCKET_LINE_MAX, 0);

		if (rcvRtn == -1)		// receive error
		{
			curlog << getLocalTimeAndPid(pthread_self()) << "Fail to receive from client."
					<< " error: " << strerror(errno) << "(errorno: " << errno << endl;
			funRtn = false;
			break;
		}
		else if (rcvRtn == 0)	// disconnect by client
		{
			curlog << getLocalTimeAndPid(pthread_self()) << "socket disconnected by client."
					<< endl;
			funRtn = false;
			break;
		}
		else					// receive successfully
		{
			loopCond = !findCRLF(rcvTmp);
			strRcv.append(rcvTmp);
			curlog << getLocalTimeAndPid(pthread_self()) << "receive data from client." << rcvTmp
					<< endl;
			curlog << getLocalTimeAndPid(pthread_self()) << "totally receive data from client."
					<< strRcv << endl;
			if (loopCond)
			{
				curlog << getLocalTimeAndPid(pthread_self()) << "receive continue." << endl;
			}
		}

	} while (loopCond);

	curlog << getLocalTimeAndPid(pthread_self()) << "function doRequest exit normally" << endl;
	return funRtn;
}

bool doResponse(int *fd, const char* recvData)
{
	using namespace std;
	ofstream &curlog = Configuration::getInstance()->getLog();
	bool rtn = true;
	char tmpCmd[SOCKET_LINE_MAX];
	memset(tmpCmd, 0, sizeof(tmpCmd));
	string sendData;
	string strDisConnect = Configuration::getInstance()->getStrDisconnect();
	string strLogOut = Configuration::getInstance()->getStrLogout();

	curlog << getLocalTimeAndPid(pthread_self()) << "function doResponse start" << endl;
	curlog << getLocalTimeAndPid(pthread_self()) << "receive from client is:" << recvData << endl;

	imapCommand curCmd = getImapCommand(recvData);
	const map<string, vector<Command> > & cmdMapQues = Configuration::getInstance()->getMapCmdQue();

	do
	{
		// if not found
		if (cmdMapQues.find(curCmd.imapCmd) == cmdMapQues.end())
		{
			curlog << getLocalTimeAndPid(pthread_self()) << "the command [" << curCmd.imapCmd
					<< "] is not found and redirected to [OTHER]:" << endl;
			curCmd.imapCmd = "OTHER";
		}

		// get command from command que
		const vector<Command> & curQues = cmdMapQues.at(curCmd.imapCmd);
		int curIndex = Configuration::getInstance()->getCmdQueIndex(curCmd.imapCmd);
		Configuration::getInstance()->cmdQueIndexIncrement(curCmd.imapCmd);
		const Command & queCmd = curQues[curIndex];

		snprintf(tmpCmd, SOCKET_LINE_MAX - 1, queCmd.cmd.c_str(), curCmd.tag.c_str(),
				curCmd.para1.c_str());
		curlog << getLocalTimeAndPid(pthread_self()) << "Execut command: " << curCmd.imapCmd << "["
				<< queCmd.id << "]:" << queCmd.name << "=" << tmpCmd << endl;
		switch (queCmd.type)
		{
			case '0':
				sendData = tmpCmd;
				break;
			case '1':
				sendData = strDisConnect + "\r\n" + tmpCmd;
				rtn = false;
				break;
			case '2':
				sendData = tmpCmd;
				rtn = false;
				break;
			case '3':	// Timeout
				cout << "Waiting for timeout" << endl;
				sleep(Configuration::getInstance()->getExeTimer());
				sendData = strDisConnect;
				rtn = false;
				break;
			case '4':
				sendData = strLogOut + "\r\n" + tmpCmd;
				rtn = false;
				break;
			default:
				sendData = "Invalid command type:" + queCmd.type + '\n' + strDisConnect;
				curlog << getLocalTimeAndPid(pthread_self()) << "Invalid command type:"
						<< queCmd.type << endl;
				rtn = false;
				break;
		}

	} while (false);

	if (!doSendBack(fd, sendData.c_str()))
	{
		rtn = false;
	}
	curlog << getLocalTimeAndPid(pthread_self()) << "function doResponse exit normally" << endl;
	return rtn;
}

/**
 * sendback data to Client
 */
bool doSendBack(int *fd, const char* sendData)
{
	using namespace std;
	int client_sockfd = *fd;
	ofstream &curlog = Configuration::getInstance()->getLog();
	string strData = string(sendData) + "\r\n";
	curlog << getLocalTimeAndPid(pthread_self()) << "function doSendBack start" << endl;
	curlog << getLocalTimeAndPid(pthread_self()) << "send back to client is:" << strData << endl;

	if (send(client_sockfd, strData.c_str(), strData.length(), 0) == -1)
	{
		curlog << getLocalTimeAndPid(pthread_self()) << "Fail to send [" << strData
				<< "] back to the client. " << " error: " << strerror(errno) << "(errorno: "
				<< errno << endl;
		curlog << getLocalTimeAndPid(pthread_self()) << "function doSendBack exit abnormally!!!"
				<< endl;
		return false;
	}
	else
	{
		cout << "SEND:" << sendData << endl;
		cout << "----------------------" << endl;
	}

	curlog << getLocalTimeAndPid(pthread_self()) << "function doSendBack exit normally" << endl;
	return true;
}

imapCommand getImapCommand(const std::string& strCmd)
{
	using namespace std;
	imapCommand rtn;
	string cmd = trim(strCmd);
	string::size_type pos = 0;
	do
	{
		//set tag
		if (cmd.length() > 0)
		{
			pos = cmd.find(' ');
			if (pos != string::npos)
			{
				rtn.tag = cmd.substr(0, pos);
				rtn.Others = trim(cmd.substr(pos, cmd.length() - pos));
				cmd = rtn.Others;
			}
			else
			{
				rtn.tag = cmd;
				break;
			}
		}
		// set command
		if (cmd.length() > 0)
		{
			pos = cmd.find(' ');
			if (pos != string::npos)
			{
				rtn.imapCmd = cmd.substr(0, pos);
				rtn.Others = trim(cmd.substr(pos, cmd.length() - pos));
				cmd = rtn.Others;
			}
			else
			{
				rtn.imapCmd = cmd;
				break;
			}
		}
		// set para1
		if (cmd.length() > 0)
		{
			pos = cmd.find(' ');
			if (pos != string::npos)
			{
				rtn.para1 = cmd.substr(0, pos);
				rtn.Others = trim(cmd.substr(pos, cmd.length() - pos));
			}
			else
			{
				rtn.para1 = cmd;
			}
		}
	} while (false);

	rtn.imapCmd = cnvToUpper(rtn.imapCmd);
	//cout << "CMD = " << rtn.imapCmd << endl;
	return rtn;
}

