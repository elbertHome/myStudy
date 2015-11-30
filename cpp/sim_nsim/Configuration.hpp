/*
 * Configuration.hpp
 *
 *  Created on: Jan 1, 2015
 *      Author: elbert
 */

#ifndef CONFIGURATION_HPP_
#define CONFIGURATION_HPP_

#include <string>
#include <vector>
#include <map>
#include <fstream>

struct Command
{
		std::string id;
		std::string name;
		char type;
		std::string cmd;
};

class Configuration
{
	public:
		// methods
		static Configuration * initInstance(const std::string& path);
		~Configuration();
		const int getCmdQueIndex(const std::string& key) const;
		void cmdQueIndexIncrement(const std::string& key);
		void show();

		static inline Configuration * getInstance()
		{
			return instance;
		}

		inline std::ofstream& getLog()
		{
			return log;
		}

		inline const std::map<std::string, Command>& getMapCmd() const
		{
			return mapCmd;
		}

		inline const std::map<std::string, std::vector<Command> >& getMapCmdQue() const
		{
			return mapCmdQue;
		}

		inline const std::string& getPort() const
		{
			return port;
		}

		inline const std::string& getStrConnect() const
		{
			return strConnect;
		}

		inline const std::string& getStrDisconnect() const
		{
			return strDisconnect;
		}

		inline const std::string& getStrLogout() const
		{
			return strLogout;
		}

		inline const int getExeTimer() const
		{
			return exeTimer;
		}

	private:
		// members
		static Configuration* instance;
		static const std::string HEAD_CONFIG;
		static const std::string HEAD_COMMANDS;
		static const std::string SERVER_PORT;
		static const std::string LOGFILE_PATH;
		static const std::string CONNECT_STR;
		static const std::string DISCONNECT_STR;
		static const std::string LOGOUT_STR;
		static const std::string EXETIMER;
		static const unsigned int CONFIG_LINE_MAX;

		std::string port;
		std::ofstream log;
		std::string strConnect;
		std::string strDisconnect;
		std::string strLogout;
		int exeTimer;
		std::map<std::string, Command> mapCmd;
		std::map<std::string, std::vector<Command> > mapCmdQue;
		std::map<std::string, int> mapCmdQueCount;

		// methods
		Configuration(const std::string& path);
		bool loadConfig(const std::string& path);
		bool loadConfigHead(std::ifstream& config, int& count);
		bool loadCommandHead(std::ifstream& config, int& count);
		bool loadCommandQues(std::ifstream& config, int& count, const std::string& head);
};

#endif /* CONFIGURATION_HPP_ */
