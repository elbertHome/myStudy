/*
 * Configuration.cpp
 *
 *  Created on: Jan 1, 2015
 *      Author: elbert
 */
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include "commontools.hpp"
#include "Configuration.hpp"

// init static member
const unsigned int Configuration::CONFIG_LINE_MAX = 1024;
Configuration* Configuration::instance = NULL;
const std::string Configuration::HEAD_CONFIG = "[CONFIGURATION]";
const std::string Configuration::HEAD_COMMANDS = "[COMMANDS]";
const std::string Configuration::SERVER_PORT = "server_port";
const std::string Configuration::LOGFILE_PATH = "logfile_path";
const std::string Configuration::CONNECT_STR = "connect_str";
const std::string Configuration::DISCONNECT_STR = "disconnect_str";
const std::string Configuration::LOGOUT_STR = "logout_str";
const std::string Configuration::EXETIMER = "exe_timer";

/**
 * get instance of Configuration class
 */
Configuration* Configuration::initInstance(const std::string& path)
{
	if (instance == NULL)
	{
		instance = new Configuration(path);
	}
	instance -> show();
	return instance;
}

void Configuration::show()
{
	using namespace std;
	if (instance == NULL)
	{
		cout << "can't show" << endl;
	}
	else
	{
		cout << SERVER_PORT << "=" << port << endl;
		cout << LOGFILE_PATH << "=" << log.is_open() << endl;
		cout << CONNECT_STR << "=" << strConnect << endl;
		cout << DISCONNECT_STR << "=" << strDisconnect << endl;
		cout << LOGOUT_STR << "=" << strLogout << endl;
		cout << EXETIMER << "=" << exeTimer << endl;

		std::map<std::string, Command>::iterator it;
		for (it = mapCmd.begin(); it != mapCmd.end(); it++)
		{
			cout << it->first << "=" << (it->second).type << ":" << (it->second).cmd << endl;
		}

		std::map<std::string, std::vector<Command> >::iterator it2;
		for (it2 = mapCmdQue.begin(); it2 != mapCmdQue.end(); it2++)
		{
			cout << it2->first << ", size=" << it2->second.size() << endl;
			std::vector<Command> ques = it2->second;
			for (unsigned int i = 0; i < ques.size(); i++)
			{
				cout << ques[i].id << "=" << ques[i].type << ":" << ques[i].cmd << ", name="
						<< ques[i].name << endl;
			}
		}
	}
	cout << "==========end of showing config file=============" << endl;
}

/**
 * private constructor
 */
Configuration::~Configuration()
{
	// close log file
	log.flush();
	log.close();

	// clear command list
	std::map<std::string, Command>::iterator it;
	for (it = mapCmd.begin(); it != mapCmd.end(); it++)
	{
		delete &(it->second);
		delete &(mapCmd.at(it->first));
	}
	mapCmd.clear();

	// clear command ques
	std::map<std::string, std::vector<Command> >::iterator it2;
	for (it2 = mapCmdQue.begin(); it2 != mapCmdQue.end(); it2++)
	{
		std::vector<Command> ques = it2->second;
		for (unsigned int i = 0; i < ques.size(); i++)
		{
			delete &ques.at(i);
		}
		delete &(it2->second);
		delete &(mapCmdQue.at(it2->first));
	}
	mapCmdQue.clear();
}

/**
 * private constructor
 */
Configuration::Configuration(const std::string& path)
{
	if (!this->loadConfig(path))
	{
		throw "Fail to open config file:" + path + "\n";
	}

}

/**
 * load total configFile
 */
bool Configuration::loadConfig(const std::string& path)
{
	std::ifstream config;		// config file
	int count = 0;
	char line[CONFIG_LINE_MAX];			// one line of the config file
	std::string strLine;		// one line of the confif file

	// open the config file
	config.open(path.c_str());
	if (!config.is_open())
	{
		printf("Fail to open config file:%s\n", path.c_str());
		return false;
	}

	memset(line, 0, CONFIG_LINE_MAX);
	// read from config file
	while (config.getline(line, CONFIG_LINE_MAX))
	{
		count++;
		strLine = line;
		strLine = trim(strLine);
		if (strLine.length() == 0 || strLine[0] == '#')
		{
			continue;
		}
		else if (strLine[0] != '[' || strLine[strLine.length() - 1] != ']')
		{
			printf("config file format error at:line(%d)%s\n", count, line);
			return false;
		}
		else if (strLine == HEAD_CONFIG)
		{
			// when configuration head
			if (!loadConfigHead(config, count))
			{
				printf("%s load error\n", HEAD_CONFIG.c_str());
				return false;
			}
		}
		else if (strLine == HEAD_COMMANDS)
		{
			// when commands head
			if (!loadCommandHead(config, count))
			{
				printf("%s load error\n", HEAD_COMMANDS.c_str());
				return false;
			}
		}
		else
		{
			// when commandques head
			if (!loadCommandQues(config, count, strLine))
			{
				printf("%s load error\n", strLine.c_str());
				return false;
			}
		}
		memset(line, 0, CONFIG_LINE_MAX);
	}
	return true;
}

/**
 * load command ques of config file
 */
bool Configuration::loadCommandQues(std::ifstream& config, int& count, const std::string & head)
{
	char line[CONFIG_LINE_MAX];	// one line of the config file
	std::string strLine;		// one line of the confif file

	std::string::size_type pos;
	std::string key;
	std::string value;
	std::string noBracketHead;
	memset(line, 0, CONFIG_LINE_MAX);

	// create a que for ever type of command
	std::vector<Command> * ques = new std::vector<Command>();
	while (config.getline(line, CONFIG_LINE_MAX))
	{
		count++;
		strLine = line;
		strLine = trim(strLine);

		if (strLine[0] == '#')
		{
			continue;
		}
		else if (strLine.length() == 0 || strLine[0] == '[')
		{
			break;
		}
		else
		{
			pos = strLine.find_first_of('=');
			if (pos == std::string::npos)
			{
				printf("config file format error at:line(%d)%s\n", count, line);
				return false;
			}
			else
			{
				// get key and value
				key = strLine.substr(0, pos);
				value = strLine.substr(pos + 1, std::string::npos);
				key = trim(key);
				value = trim(value);
				if (key.length() == 0 || value.length() == 0)
				{
					printf("config file format error at:line(%d)%s\n", count, line);
					return false;
				}
				else
				{
					Command* item = new Command();
					item->id = key;
					if (value[value.length() - 1] == '*')
					{
						value = value.substr(0, value.length() - 1);
						if (mapCmd.find(value) != mapCmd.end())
						{
							item->type = '2';
							item->cmd = mapCmd.at(value).cmd;
							item->name = value;
						}
						else
						{
							printf("config file format error at:line(%d)%s\n", count, line);
							return false;
						}
					}
					else
					{
						if (mapCmd.find(value) != mapCmd.end())
						{
							item->type = mapCmd.at(value).type;
							item->cmd = mapCmd.at(value).cmd;
							item->name = value;
						}
						else
						{
							printf("config file format error at:line(%d)%s\n", count, line);
							return false;
						}
					}
					ques->push_back(*item);
				}
			}
		}
		memset(line, 0, CONFIG_LINE_MAX);
	} // end of while;
	  // delete [ ]
	noBracketHead = head.substr(1, head.length() - 2);
	std::pair<std::string, std::vector<Command> > * item_map = new std::pair<std::string,
			std::vector<Command> >(noBracketHead, *ques);
	mapCmdQue.insert(*item_map);

	std::pair<std::string, int> item_cnt = std::pair<std::string, int>(noBracketHead, 0);
	mapCmdQueCount.insert(item_cnt);

	return true;
}

/**
 * load command head of config file
 */
bool Configuration::loadCommandHead(std::ifstream& config, int& count)
{
	char line[CONFIG_LINE_MAX];	// one line of the config file
	std::string strLine;	// one line of the confif file

	std::string::size_type pos;
	std::string key;
	std::string value;
	memset(line, 0, CONFIG_LINE_MAX);
	while (config.getline(line, CONFIG_LINE_MAX))
	{
		count++;
		strLine = line;
		strLine = trim(strLine);

		if (strLine[0] == '#')
		{
			continue;
		}
		else if (strLine.length() == 0 || strLine[0] == '[')
		{
			break;
		}
		else
		{
			pos = strLine.find_first_of('=');
			if (pos == std::string::npos)
			{
				printf("config file format error at:line(%d)%s\n", count, line);
				return false;
			}
			else
			{
				// get key and value
				key = strLine.substr(0, pos);
				value = strLine.substr(pos + 1, std::string::npos);
				key = trim(key);
				value = trim(value);
				if (key.length() == 0 || value.length() == 0)
				{
					printf("config file format error at:line(%d)%s\n", count, line);
					return false;
				}
				else
				{
					Command * item_cmd = new Command;
					item_cmd->id = "unknown";
					item_cmd->name = key;
					item_cmd->type = value[0];
					item_cmd->cmd = value.substr(2, value.length() - 2);
					std::pair<std::string, Command> * item_map =
							new std::pair<std::string, Command>(key, *item_cmd);
					mapCmd.insert(*item_map);
				}
			}
		}

		memset(line, 0, CONFIG_LINE_MAX);
	}		// end of while

	return true;
}

/**
 * load configuration head of config file
 */
bool Configuration::loadConfigHead(std::ifstream & config, int& count)
{
	char line[CONFIG_LINE_MAX];		// one line of the config file
	char hostname[CONFIG_LINE_MAX];	// hostname of the server
	std::string strLine;			// one line of the confif file

	std::string::size_type pos;
	std::string key;
	std::string value;
	memset(line, 0, CONFIG_LINE_MAX);
	memset(hostname, 0, CONFIG_LINE_MAX);
	if (gethostname(hostname, CONFIG_LINE_MAX) == -1)
	{
		printf("gethostname error at:line(%d)\n", count);
		return false;
	}
	while (config.getline(line, CONFIG_LINE_MAX))
	{
		count++;
		strLine = line;
		strLine = trim(strLine);

		if (strLine[0] == '#')
		{
			continue;
		}
		else if (strLine.length() == 0 || strLine[0] == '[')
		{
			break;
		}
		else
		{
			pos = strLine.find_first_of('=');
			if (pos == std::string::npos)
			{
				printf("config file format error at:line(%d)%s\n", count, line);
				return false;
			}
			else
			{
				// get key and value
				key = strLine.substr(0, pos);
				value = strLine.substr(pos + 1, std::string::npos);
				key = trim(key);
				value = trim(value);

				if (key.length() == 0 || value.length() == 0)
				{
					printf("config file format error at:line(%d)%s\n", count, line);
					return false;
				}
				else if (key == SERVER_PORT)
				{
					this->port = value;
				}
				else if (key == LOGFILE_PATH)
				{
					log.open(value.c_str(), std::ios_base::app);
					if (!log.is_open())
					{
						printf("Fail to open log file:%s\n", value.c_str());
						return false;
					}
				}
				else if (key == CONNECT_STR)
				{
					sprintf(line, value.c_str(), hostname, getlogin());
					this->strConnect = line;
				}
				else if (key == DISCONNECT_STR)
				{
					sprintf(line, value.c_str(), hostname, getlogin());
					this->strDisconnect = line;
				}
				else if (key == LOGOUT_STR)
				{
					this->strLogout = value;
				}
				else if (key == EXETIMER)
				{
					this->exeTimer = atoi(value.c_str());
				}
				else
				{
					printf("config file format error at:line(%d)%s\n", count, line);
					return false;
				}
			}
		}
		memset(line, 0, CONFIG_LINE_MAX);
	}		// end of while
	return true;
}

const int Configuration::getCmdQueIndex(const std::string& key) const
{
	if (mapCmdQueCount.find(key) != mapCmdQueCount.end())
	{
		return mapCmdQueCount.at(key);
	}
	else
	{
		return -1;
	}
}

void Configuration::cmdQueIndexIncrement(const std::string& key)
{
	if (mapCmdQueCount.find(key) != mapCmdQueCount.end())
	{
		unsigned int curIndex = mapCmdQueCount.at(key);
		if (curIndex < mapCmdQue.at(key).size() - 1)
		{
			mapCmdQueCount[key]++;
		}
		else
		{
			mapCmdQueCount[key] = 0;
		}
	}
}

